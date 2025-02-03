#include "CSEOptimizer.h"
#include "SimpleNode.h"
#include "ExpressionNode.h"
#include "MovNode.h"
#include "DeclareNode.h"
#include "PropagationOptimizer.h"
#include <set>
#include <stack>


CSEOptimizer::CSEOptimizer(BasicBlock* entryBasicBlock, int nextProgramTempVariableCount) : entryBasicBlock(entryBasicBlock), nextProgramTempVariableCount(nextProgramTempVariableCount) {
}

bool CSEOptimizer::iterateCommonSubexpressionElimination()
{
    bool modified = commonSubexpressionEliminationOnce();
    ///UNCOMMENT: to run CSE multiple times, but I don't think it is necessary
    // bool runCSE = true;
    // while (runCSE)
    // {
    //     runCSE = commonSubexpressionEliminationOnce();
    //     //if modified, rerun again to see any more common subexpressions
    // }
    return modified;
}

bool CSEOptimizer::iterateCSE_CopyPropagation()
{
    bool modified = true;

    while (modified) {
        modified = commonSubexpressionEliminationOnce();
        PropagationOptimizer propagationOptimizer(entryBasicBlock);
        modified |= propagationOptimizer.runCopyPropagation();
    }
    return modified;
}


bool CSEOptimizer::commonSubexpressionEliminationOnce()
{
    // need to reinitialise the basic blocks and avail sets each time, since it might have changed since the last iteration
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);

    // Analysis: AVAIL
    AVAIL avail(entryBasicBlock);
    nodeAvailSets = avail.getNodeInDataFlowSets();
    basicBlocks = avail.getBasicBlocksUsed();

    // Transformation: Remove common subexpressions
    bool removed = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        removed |= basicBlockRemoveCommonSubexpressions(basicBlock);
    }
    return removed;
}


int CSEOptimizer::getNextProgramTempVariableCount()
{
    return nextProgramTempVariableCount;
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

    //now we have the in_avail set, for each instruction, check if the current instruction generates an expression which is already available
    //then we calculate the out_avail set for the instruction (which will be the in_avail set for the next instruction)

    std::list<std::weak_ptr<BaseNode>>& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end();) {
        //for each instruction, check if it generates an expression which is already available

        std::shared_ptr<BaseNode> instruction = it->lock();
        std::set<std::string> in_avail_set = nodeAvailSets[instruction];   //get the in_avail set for the instruction
        std::set<std::string> generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(instruction);
        
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
            std::shared_ptr<SimpleNode> simpleNode = std::dynamic_pointer_cast<SimpleNode>(instruction);
            if (simpleNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not a SimpleNode subclass");
            }

            ///FIRST: replace the current instruction y:=e
            //with a MOV instruction with a new temporary variable y:= t
            std::string tempVar = addNewProgramTempVariable();     //Also adds a declaration!!
            assert(simpleNode->getDefinedVariables().size() == 1);
            std::string destVar = *simpleNode->getDefinedVariables().begin();  //the variable defined, i.e. y
            std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(destVar,tempVar);
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
    }

    return modified;
}

bool CSEOptimizer::basicBlockBackwardsFindAndReplaceExpression(BasicBlock* basicBlock, std::string &expressionToFind, std::string &tempVar, std::shared_ptr<BaseNode> beginBackwardsFromThisNode)
{
    //scan upwards, find the first occurrence of the expression z:=expr being generated by an instruction
    //replace it with a the temporary variable t:=expr
    //add a new MOV instruction after that to set z:=t
    //return true if the expression is found and replaced, false otherwise
    std::list<std::weak_ptr<BaseNode>>& instructions = basicBlock->get_instructions_reference();

    instructions.reverse();

    std::list<std::weak_ptr<BaseNode>>::iterator it2;
    if (beginBackwardsFromThisNode == nullptr) {
        it2 = instructions.begin();        //default start from the end of the basic block
    } else {
        it2 = std::find_if(instructions.begin(), instructions.end(), [beginBackwardsFromThisNode](std::weak_ptr<BaseNode> node) {
            return node.lock() == beginBackwardsFromThisNode;
        });     //start backwards search from the beginBackwardsFromThisNode
    }

    for (; it2 != instructions.end(); ++it2) {    //reverse iterator
        std::shared_ptr<BaseNode> instruction = it2->lock();
        std::set<std::string> generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(instruction);

        if (generatedExpressions.find(expressionToFind) != generatedExpressions.end()) {
            //expression found, EDIT

            //first just edit the existing expression node, and change the dest to the temp variable
            std::shared_ptr<ExpressionNode> expressionNode = std::dynamic_pointer_cast<ExpressionNode>(instruction);
            if (expressionNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not an ExpressionNode subclass");
            }
            std::string originalDest = expressionNode->getDest();
            expressionNode->setDest(tempVar);       //change the dest to the temp variable

            // after the current expression node t:=expr,
            // insert a new MOV instruction into the IR tree to set z:=t
            std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(originalDest, tempVar);
            it2 = basicBlock->insert_sandwich_instruction_node(it2, movNode, true);    //updates the iterator to point to the next instruction - don't really need it
            instructions.reverse();    //reverse back to original order before returning and inserting to the start of the BasicBlock
            return true;      //expression found and replaced
        }
    }
    instructions.reverse();    //reverse back to original order before returning
    return false;       //expression not found - continue DFS search backwards
}

std::string CSEOptimizer::addNewProgramTempVariable()
{
    //increment nextProgramTempVariableCount after creating the new program temp variable
    std::string programTempVar = "_s" + std::to_string(nextProgramTempVariableCount++);

    //add a declare statement to the start of the program
    std::shared_ptr<DeclareNode> declareTempVarNode = std::make_shared<DeclareNode>(programTempVar);

    //get the first instruction of the program from the first basic block
    ///WARNING: this assumes the instructions are in normal order, not reversed
    std::list<std::weak_ptr<BaseNode>>::iterator declarationIterator = entryBasicBlock->get_instructions_reference().begin();
    entryBasicBlock->insert_sandwich_instruction_node(declarationIterator, declareTempVarNode, false);
    
    return programTempVar;
}