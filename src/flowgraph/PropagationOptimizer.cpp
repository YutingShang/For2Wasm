#include "PropagationOptimizer.h"
#include "ACS.h"
#include "MovNode.h"
#include "BasicBlock.h"
#include <stack>
#include <iostream>


PropagationOptimizer::PropagationOptimizer(BasicBlock *entryBasicBlock) : entryBasicBlock(entryBasicBlock) {}

bool PropagationOptimizer::runCopyPropagation()
{
    return runPropagation(PropagationType::COPY_PROPAGATION);
}

bool PropagationOptimizer::runConstantPropagation() {
    return runPropagation(PropagationType::CONSTANT_PROPAGATION);
}

bool PropagationOptimizer::runPropagation(PropagationType propagationType) {
    // get the available copy statements for each basic block
    // Analysis: Available Copy Statements (ACS)
    ACS acs(entryBasicBlock);
    basicBlocks = acs.getBasicBlocksUsed();
    nodeAvailCopies = acs.getNodeDataFlowSets();

    // Transformation: Copy Propagation
    bool modified = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        //if one of the basic blocks is modified, then set modified to true
        modified |= basicBlockPropagation(basicBlock, propagationType);
    }
    return modified;
}

bool PropagationOptimizer::basicBlockPropagation(BasicBlock *basicBlock, PropagationType propagationType)
{
    bool modified = false;

    // iterate through the instructions in the basic block, starting from the top
    /// PSUEDOCODE:
    // compute & check the IN-availCopies set of each instruction
    // for referenced variables {x} in the instruction node
    // if the variable is in the IN-availCopies set, and there is a copy statement (x, *) available
        //for constant propagation, if (x, c), then replace x with c
        //for copy propagation, if (x, y), then recursively find any (y, *), repeat to replace with final replacement variable
        ///IMPORTANT:* also only replace the variable if its var or replacement is NOT a _t temporary variable
            // (i.e. do NOT replace (x ,_t0) since temp variables cannot be stored and retrieved in wasm)
            // (i.e. if I replace (_t0, a), this allows for further optimisations, but I need to search upwards and delete the MOV _t0 a instruction)


    // now for each instruction, check its in-availCopies set as specified in the pseudo code above

    std::list<BaseNode *> &instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end();)
    {
        BaseNode *instruction = *it;
        std::set<std::pair<std::string, std::string>> inAvailCopiesSet = nodeAvailCopies[instruction];  //get the in-availCopies set for the current instruction
        std::set<std::string> referencedVariables = instruction->getReferencedVariables();
        std::set<std::pair<std::string, std::string>> generatedCopyStatements = AnalysisTools::getGeneratedCopyStatementsAtNode(instruction);


        //for every referenced variable in the instruction, check if it is in the in-availCopies set like (x, *)
        // I think if a pair like (x, *) exists, then there will only be one such pair in the set - hence use a WHILE loop
        // Because availCopyStatements uses intersection and new copy statements replace the old ones
        for (const std::string &var : referencedVariables)      //for each referenced variable x
        {
            //iterate through the in-availCopies set
            auto tuplesIt = inAvailCopiesSet.begin();
            bool found = false;
            while (tuplesIt != inAvailCopiesSet.end() && !found)     //find the pair (x, *)
            {
                std::pair<std::string, std::string> tuple = *tuplesIt;
                if (tuple.first == var)         //check it is (x, *)
                {
                    found = true;

                    //for constant propagation, if (x, c), then replace x with c
                    if (propagationType == PropagationType::CONSTANT_PROPAGATION){
                        if (isConstant(tuple.second)){
                            //replace the variable with the definition
                            //actually modify the instruction node
                            instruction->replaceReferencedVariable(var, tuple.second);
                            
                            if (isInternalTemporaryVariable(tuple.first)){  //if replacing a temporary variable, go back and remove the (MOV _t0 y) instruction
                                removeMovTempInstruction(basicBlock, instruction, tuple.first);
                            }
                            modified = true;
                        }
                    }
                    //for copy propagation, if (x, y), then recursively find any (y, *), repeat to replace with final replacement variable
                    else if (propagationType == PropagationType::COPY_PROPAGATION){
                        if (!isConstant(tuple.second)){     //check it is NOT (x, c), but is (x, y)
                            //see if there are any (y, *) in the in-availCopies set, iterate till final replacement variable
                            std::string replacementVar = getFinalReplacementVariable(tuple.second, inAvailCopiesSet); 
                            
                            if (!isInternalTemporaryVariable(replacementVar)){  // Handles the case (x, _t0), we don't replace
                                //actually modify the instruction node
                                instruction->replaceReferencedVariable(var, replacementVar);
                                
                                if (isInternalTemporaryVariable(tuple.first)){  //if replacing a temporary variable, go back and remove the (MOV _t0 y) instruction
                                    removeMovTempInstruction(basicBlock, instruction, tuple.first);
                                }
                                modified = true;
                            }
                        }
                    }
                    
                }
                tuplesIt++;   //check the next tuple to find (x, *)
            } 
        }

        //after performing propagation on the current instruction, update the in-availCopies set for the next instruction
        ///NOTE: I got the generated and killed copy statements before modifying the current instruction, so its like the original ACS analysis

        //increment the iterator to the next instruction
        ++it;

    }

    return modified;
}

std::string PropagationOptimizer::getFinalReplacementVariable(std::string var, std::set<std::pair<std::string, std::string>> &inAvailCopiesSet) {

    //follow the chain of variables (temp or non-temp) until no more replacement variables are found or a constant is reached
    //then return the final NON-TEMPORARY variable
    // e.g. x-> _t0 -> _t1 -> b -> _t2 returns b

    bool moreReplacementsFound = true;     //no more mappings found, or a constant is reached
    std::string nextVarToFind = var;
    std::string lastNonTempVar = var;    //last non-temporary variable found, if its a chain of temp variables, then return the original variable

    while (moreReplacementsFound) {
        //find the (y, *) in the in-availCopies set
        auto tuplesIt = inAvailCopiesSet.begin();
        moreReplacementsFound = false;    //reset to false for this iteration

        //iterate through map to find (nextVarToFind, *)
        while (tuplesIt != inAvailCopiesSet.end()) {
            std::pair<std::string, std::string> tuple = *tuplesIt;
            if (tuple.first == nextVarToFind) {       //check for (y, z)
                moreReplacementsFound = true;

                if (isConstant(tuple.second)){     //z is a constant - stop mapping
                    return lastNonTempVar;
                }
                else if (isInternalTemporaryVariable(tuple.second)){    //z is a temp variable - continue mapping
                    nextVarToFind = tuple.second;
                }
                else {      //z is a non-temp variable - continue mapping
                    lastNonTempVar = tuple.second;
                    nextVarToFind = tuple.second;
                }
            }
            tuplesIt++;
        }
    }

    return lastNonTempVar;
}

void PropagationOptimizer::removeMovTempInstruction(BasicBlock *startSearchFromBasicBlock, BaseNode *startSearchFromInstructionNode, std::string tempVar) {
    ///On a backwards control flow, find the (MOV _t0 a) instruction, and remove it
    // do a DFS backwards from the current basic block as the start node

    std::stack<BasicBlock *> toExploreStack;
    toExploreStack.push(startSearchFromBasicBlock);
    std::set<BasicBlock *> seen;
    seen.insert(startSearchFromBasicBlock);

    //should only be one temp variable MOV instruction, so we stop immediately if found
    bool expressionFound = false;
    while (!toExploreStack.empty() && !expressionFound) {
        BasicBlock *current = toExploreStack.top();
        toExploreStack.pop();
        //go inside each basic block
        
        if (current == startSearchFromBasicBlock) {      //only for the first basic block, start backwards search from the current instruction
            //beginBackwardsFromThisNode is the current instruction node (had copy propagation applied to it)
            expressionFound = basicBlockRemoveMovTempInstruction(current, tempVar, startSearchFromInstructionNode);
        } else {             //for all other basic blocks, start backwards search from the end of the basic block
            expressionFound = basicBlockRemoveMovTempInstruction(current, tempVar);
        }
        
        if (!expressionFound){    //then we add the predecessors to the stack 
            std::vector<BasicBlock*> predecessors = current->get_predecessors();
            //check that there should be at least one predecessor, otherwise we have hit the entry instruction node on a backwards path
            if (predecessors.empty()) {
                throw std::runtime_error("ERROR while handling propagation: expression for MOV " + tempVar + " not found on a backwards path");
            }
            for (BasicBlock* predecessor : predecessors) {
                if (seen.find(predecessor) == seen.end()) {     //if it is not already seen
                    toExploreStack.push(predecessor);
                    seen.insert(predecessor);
                }
            }
        }
    }
}

bool PropagationOptimizer::basicBlockRemoveMovTempInstruction(BasicBlock *basicBlock, std::string tempVar, BaseNode* beginBackwardsFromThisNode) {
    bool found = false;
    
    std::list<BaseNode *> &instructions = basicBlock->get_instructions_reference();
    instructions.reverse();
    
    std::list<BaseNode*>::iterator it2;
    if (beginBackwardsFromThisNode == nullptr) {
        it2 = instructions.begin();        //default start from the end of the basic block
    } else {
        it2 = std::find(instructions.begin(), instructions.end(), beginBackwardsFromThisNode);     //start backwards search from the beginBackwardsFromThisNode
    }

    //find the (MOV _t0 a) instruction and remove it if it exists
    for (auto it = instructions.begin(); it != instructions.end(); ++it) {
        BaseNode *instruction = *it;
        if (dynamic_cast<MovNode *>(instruction) != nullptr) {
            MovNode *movNode = dynamic_cast<MovNode *>(instruction);
            if (movNode->getDest() == tempVar) {
                basicBlock->remove_instruction_node(it);
                found = true;
                break;
            }
        }
    }
    //reverse back to original order
    instructions.reverse();

    return found;
}


bool PropagationOptimizer::isInternalTemporaryVariable(std::string var) {
    return var[0] == '_' && var[1] == 't';
}

bool PropagationOptimizer::isConstant(std::string var) {
    //checks if it is a positive integer
    return std::all_of(var.begin(), var.end(), ::isdigit);
}