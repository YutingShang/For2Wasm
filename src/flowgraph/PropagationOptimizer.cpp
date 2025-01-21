#include "PropagationOptimizer.h"
#include "ACS.h"
#include "MovNode.h"
#include "BasicBlock.h"
#include <stack>
#include <iostream>


PropagationOptimizer::PropagationOptimizer(BasicBlock *entryBasicBlock) : entryBasicBlock(entryBasicBlock) {}

void PropagationOptimizer::runCopyPropagation()
{
    runPropagation(PropagationType::COPY_PROPAGATION);
}

void PropagationOptimizer::runConstantPropagation() {
    runPropagation(PropagationType::CONSTANT_PROPAGATION);
}

void PropagationOptimizer::runPropagation(PropagationType propagationType) {
    // get the available copy statements for each basic block
    // Analysis: Available Copy Statements (ACS)
    ACS acs(entryBasicBlock);
    basicBlocks = acs.getBasicBlocksUsed();
    availCopies = acs.getAvailCopies();
    allCopyStatements = acs.getAllCopyStatements();

    // Transformation: Copy Propagation
    for (BasicBlock *basicBlock : basicBlocks)
    {
        basicBlockCopyPropagation(basicBlock, propagationType);
    }
}

bool PropagationOptimizer::basicBlockCopyPropagation(BasicBlock *basicBlock, PropagationType propagationType)
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


    std::set<std::pair<std::string, std::string>> inAvailCopiesSet = allCopyStatements;


    // in-availCopies for the first instruction is the intersection of the out-availCopies of all the predecessors
    std::vector<BasicBlock *> predecessors = basicBlock->get_predecessors();
    for (BasicBlock *predecessor : predecessors)
    {
        // get the availCopies set for the predecessor
        int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
        std::set<std::pair<std::string, std::string>> predecessorAvailCopies = availCopies[predecessorIndex];

        //intersect the predecessor availCopies set with the availCopies set
        std::set<std::pair<std::string, std::string>> intersectedAvailCopies;
        std::set_intersection(inAvailCopiesSet.begin(), inAvailCopiesSet.end(), predecessorAvailCopies.begin(), predecessorAvailCopies.end(), std::inserter(intersectedAvailCopies, intersectedAvailCopies.begin()));
        inAvailCopiesSet = intersectedAvailCopies;

    } // we have the in-availCopies set for the first instruction

    // now for each instruction, check its in-availCopies set as specified in the pseudo code above

    std::list<BaseNode *> &instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end();)
    {
        BaseNode *instruction = *it;
        std::set<std::string> referencedVariables = instruction->getReferencedVariables();
        std::set<std::pair<std::string, std::string>> generatedCopyStatements = AnalysisTools::getGeneratedCopyStatementsAtNode(instruction);
        std::set<std::pair<std::string, std::string>> killedCopyStatements = AnalysisTools::getKilledCopyStatementsAtNode(instruction, allCopyStatements);


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
                            //go back and remove the (MOV _t0 c) instruction
                            if (isInternalTemporaryVariable(tuple.first)){
                                removeMovTempInstruction(basicBlock, instruction, tuple.first);
                            }
                            modified = true;
                        }
                    }
                    //for copy propagation, if (x, y), then recursively find any (y, *), repeat to replace with final replacement variable
                    else if (propagationType == PropagationType::COPY_PROPAGATION){
                        if (!isInternalTemporaryVariable(tuple.second) && !isConstant(tuple.second)){     //check it is NOT (x, _t0) or (x, c), but is (x, y)
                            //see if there are any (y, *) in the in-availCopies set, iterate till final non-temporary variable
                            std::string replacementVar = getFinalReplacementVariable(tuple.second, inAvailCopiesSet); 
                            //actually modify the instruction node
                            instruction->replaceReferencedVariable(var, replacementVar);
                            //go back and remove the (MOV _t0 y) instruction
                            if (isInternalTemporaryVariable(tuple.first)){
                                removeMovTempInstruction(basicBlock, instruction, tuple.first);
                            }
                            modified = true;
                        }
                    }
                    
                }
                tuplesIt++;   //check the next tuple to find (x, *)
            } 
        }

        //after performing propagation on the current instruction, update the in-availCopies set for the next instruction
        ///NOTE: I got the generated and killed copy statements before modifying the current instruction, so its like the original ACS analysis

        //remove the killed copy statements from the avail set
        for (std::pair<std::string, std::string> copyStatement : killedCopyStatements) {
            inAvailCopiesSet.erase(copyStatement);
        }

        //add the generated expressions to the avail set
        for (std::pair<std::string, std::string> copyStatement : generatedCopyStatements) {
            inAvailCopiesSet.insert(copyStatement);
        }

        //increment the iterator to the next instruction
        ++it;

    }

    return modified;
}

std::string PropagationOptimizer::getFinalReplacementVariable(std::string var, std::set<std::pair<std::string, std::string>> &inAvailCopiesSet) {
    std::string finalReplacementVar = var;     //e.g. variable y

    //iterate through the in-availCopies set, find the final non-temporary variable replacement
    //note - only for copy propagation, so stops before a constant copy statement is reached
    bool reachedTempOrConst = false;
    bool replacementFound = true;

    while (!reachedTempOrConst && replacementFound) {   

        //try to find a (y, *) in the in-availCopies set
        auto tuplesIt = inAvailCopiesSet.begin();
        replacementFound = false;
        while (tuplesIt != inAvailCopiesSet.end() && !reachedTempOrConst) {
            std::pair<std::string, std::string> tuple = *tuplesIt;

            if (tuple.first == finalReplacementVar) {       //check for (y, _)
                replacementFound = true;
                if (isInternalTemporaryVariable(tuple.second) || isConstant(tuple.second)){
                    reachedTempOrConst = true;       //check its not (y, _t0) or (y, c). In which case return y
                }
                else {
                    finalReplacementVar = tuple.second;    //otherwise (y, z), so replace y with z
                }
            }
            tuplesIt++;
        }
        //if no (y, *) found, then stop the loop, and return y

    }
    return finalReplacementVar;
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