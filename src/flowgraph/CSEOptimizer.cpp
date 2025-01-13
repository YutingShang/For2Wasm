#include "CSEOptimizer.h"
#include "SimpleNode.h"
#include "ExpressionNode.h"
#include "MovNode.h"
#include "DeclareNode.h"
#include <set>
#include <stack>


CSEOptimizer::CSEOptimizer(BasicBlock* entryBasicBlock) : entryBasicBlock(entryBasicBlock) {
}

void CSEOptimizer::iterateCommonSubexpressionElimination()
{

    bool runCSE = true;
    while (runCSE)
    {
        runCSE = commonSubexpressionEliminationOnce();
        //if modified, rerun again to see any more common subexpressions
    }

}


bool CSEOptimizer::commonSubexpressionEliminationOnce()
{
    // need to reinitialise the basic blocks and avail sets each time, since it might have changed since the last iteration
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);

    // Analysis: AVAIL
    AVAIL avail(entryBasicBlock);
    availSets = avail.getAvailSets();
    basicBlocks = avail.getBasicBlocks();
    allExpressions = avail.getAllExpressions();

    // Transformation: Remove common subexpressions
    bool removed = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        removed |= basicBlockRemoveCommonSubexpressions(basicBlock);
    }

    return removed;
}


bool CSEOptimizer::basicBlockRemoveCommonSubexpressions(BasicBlock *basicBlock)
{
    //iterate through the instructions in the basic block
    //if the current instruction generates an expression which is already available,
        // i.e. check the (in)avail set before the corresponding instruction
    // if the expression is in the (in)avail set, then deal with it in CSE
        // first replace that instruction with a MOV instruction with a temporary variable
        // on every control flow backwards, find the first occurrence of that expression (NOTE: maybe case where 2 backward paths merge)
        // and add a new MOV instruction to save the expression into the temporary variable
   
    //return true if the basic block has been modified, false otherwise
    bool modified = false;

    //in_avail set is the intersection of all the predecessors' (out)avail sets
    std::set<std::string> in_avail_set = allExpressions;
    //(most copy code from basicBlockComputeAvailSet)
    std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
    if (predecessors.empty()) {
        in_avail_set = {};
    } else {
        for (BasicBlock* predecessor : predecessors) {
   
            //get the avail[p] for the predecessor
            int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
            std::set<std::string> predecessorAvailSet = availSets[predecessorIndex];

            //intersect the predecessor avail set with the avail set, store in availSet
            std::set<std::string> intersectedAvailSet;
            std::set_intersection(in_avail_set.begin(), in_avail_set.end(), predecessorAvailSet.begin(), predecessorAvailSet.end(), std::inserter(intersectedAvailSet, intersectedAvailSet.begin()));
            in_avail_set = intersectedAvailSet;
        }
    } 

    //now we have the in_avail set, for each instruction, check if the current instruction generates an expression which is already available
    //then we calculate the out_avail set for the instruction (which will be the in_avail set for the next instruction)

    std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end();) {
        //for each instruction, check if it generates an expression which is already available

        BaseNode* instruction = *it;
        std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();
        std::set<std::string> killedExpressions = AnalysisTools::getKilledExpressionsAtNode(instruction, allExpressions);

        if (generatedExpressions.empty()) {
            //the instruction does not generate any expressions
            ++it;
            continue;
        }
        //otherwise, there is at least one expression generated
        assert(generatedExpressions.size() == 1);     //should just be one expression that is generated, which we need to handle if it is already available
        std::string expr = *generatedExpressions.begin();

        if (in_avail_set.find(expr) != in_avail_set.end()) {    
            modified = true;
            //current instruction DOES generates an available expression!
            //handle with CSE transformation!
            
            //the instruction should be ArithmeticNode, LogicBinNode, LogicNotNode, or RelOpNode
            //these are all SimpleNode subclasses!!
            SimpleNode* simpleNode = dynamic_cast<SimpleNode*>(instruction);
            if (simpleNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not a SimpleNode subclass");
            }

            ///FIRST: replace the current instruction y:=e
            //with a MOV instruction with a new temporary variable y:= t
            std::string tempVar = addNewProgramTempVariable();     //Also adds a declaration!!
            assert(simpleNode->getDefinedVariables().size() == 1);
            std::string destVar = *simpleNode->getDefinedVariables().begin();  //the variable defined, i.e. y
            MovNode* movNode = new MovNode(destVar,tempVar);
            it = basicBlock->replace_instruction_node(it, movNode);    //updates the iterator to point to the next instruction
  
            ///SECOND: for every backwards control flow, find the first occurrence of the expression, and EDIT
            // do a DFS backwards from the current basic block as the start node
                //need function to go inside each basic block
            std::stack<BasicBlock *> toExploreStack;
            toExploreStack.push(basicBlock);
            std::set<BasicBlock *> seen;
            seen.insert(basicBlock);

            while (!toExploreStack.empty()) {
                BasicBlock *current = toExploreStack.top();
                toExploreStack.pop();
                //go inside each basic block
                
                bool expressionFound = false;
                if (current == basicBlock) {      //only for the first basic block, start backwards search from the current instruction
                    //beginBackwardsFromThisNode is the current instruction node (which was replaced with the movNode)
                    expressionFound = basicBlockBackwardsFindAndReplaceExpression(current, expr, tempVar, movNode);
                } else {             //for all other basic blocks, start backwards search from the end of the basic block
                    expressionFound = basicBlockBackwardsFindAndReplaceExpression(current, expr, tempVar);
                }
                
                if (!expressionFound){    //then we add the predecessors to the stack 
                    std::vector<BasicBlock*> predecessors = current->get_predecessors();
                    //check that there should be at least one predecessor, otherwise we have hit the entry instruction node on a backwards path
                    if (predecessors.empty()) {
                        throw std::runtime_error("ERROR while handling CSE: expression " + expr + " not found on a backwards path");
                    }
                    for (BasicBlock* predecessor : predecessors) {
                        if (seen.find(predecessor) == seen.end()) {     //if it is not already seen
                            toExploreStack.push(predecessor);
                            seen.insert(predecessor);
                        }
                    }
                }
                //otherwise we prune the DFS search up that backwards path (don't add the predecessors to the stack)
            }
            //iterator already points to the next instruction from replace_instruction_node
        }else{
            //the current instruction does not generate an available expression
            //so we just continue to the next instruction
            ++it;
        }

        //calculate the new (in)avail set for the next instruction node
        for (std::string expr : killedExpressions) {
            in_avail_set.erase(expr);
        }

        for (std::string expr : generatedExpressions) {
            in_avail_set.insert(expr);
        }
    }

    return modified;
}

bool CSEOptimizer::basicBlockBackwardsFindAndReplaceExpression(BasicBlock* basicBlock, std::string &expressionToFind, std::string &tempVar, BaseNode* beginBackwardsFromThisNode)
{
    //scan upwards, find the first occurrence of the expression z:=expr being generated by an instruction
    //replace it with a the temporary variable t:=expr
    //add a new MOV instruction after that to set z:=t
    //return true if the expression is found and replaced, false otherwise
    std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();

    instructions.reverse();

    std::list<BaseNode*>::iterator it2;
    if (beginBackwardsFromThisNode == nullptr) {
        it2 = instructions.begin();        //default start from the end of the basic block
    } else {
        it2 = std::find(instructions.begin(), instructions.end(), beginBackwardsFromThisNode);     //start backwards search from the beginBackwardsFromThisNode
    }

    for (; it2 != instructions.end(); ++it2) {    //reverse iterator
        BaseNode* instruction = *it2;
        std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();

        if (generatedExpressions.find(expressionToFind) != generatedExpressions.end()) {
            //expression found, EDIT

            //first just edit the existing expression node, and change the dest to the temp variable
            ExpressionNode* expressionNode = dynamic_cast<ExpressionNode*>(instruction);
            if (expressionNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not an ExpressionNode subclass");
            }
            std::string originalDest = expressionNode->getDest();
            expressionNode->setDest(tempVar);       //change the dest to the temp variable

            // after the current expression node t:=expr,
            // insert a new MOV instruction into the IR tree to set z:=t
            MovNode* movNode = new MovNode(originalDest, tempVar);
            it2 = basicBlock->insert_sandwich_instruction_node(it2, movNode);    //updates the iterator to point to the next instruction - don't really need it
            instructions.reverse();    //reverse back to original order before returning and inserting to the start of the BasicBlock
            return true;      //expression found and replaced
        }
    }
    instructions.reverse();    //reverse back to original order before returning
    return false;       //expression not found - continue DFS search backwards
}

std::string CSEOptimizer::addNewProgramTempVariable()
{
    std::string programTempVar = "_s" + std::to_string(nextProgramTempVariableCount++);

    //add a declare statement to the start of the program
    DeclareNode* declareTempVarNode = new DeclareNode(programTempVar);

    //get the first instruction of the program from the first basic block
    ///WARNING: this assumes the instructions are in normal order, not reversed
    std::list<BaseNode*>::iterator declarationIterator = entryBasicBlock->get_instructions_reference().begin();
    entryBasicBlock->insert_sandwich_instruction_node(declarationIterator, declareTempVarNode);
    
    return programTempVar;
}