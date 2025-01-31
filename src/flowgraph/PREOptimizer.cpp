#include "PREOptimizer.h"
#include "VBE.h"
#include "AVAIL_PRE.h"
#include "POST.h"
#include "USED.h"
#include "IrFlowgraphVisitor.h"
#include "EndBlockNode.h"
#include "InsertableBasicBlock.h"
#include "TestNode.h"
#include "LoopNode.h"
#include "IfNode.h"
#include "DeclareNode.h"
#include "PlaceholderNode.h"
#include "MovNode.h"


PREOptimizer::PREOptimizer(BasicBlock* entryBasicBlock, int nextProgramTempVariableCount) : entryBasicBlock(entryBasicBlock), nextProgramTempVariableCount(nextProgramTempVariableCount) {
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    this->allExpressionsToCloneableNodesMap = AnalysisTools::getAllProgramExpressionsToCloneableNodesMap(rootNode);
    //get all expressions in the program, just take the keys from the map instead of traversing the IR tree again
    for (auto it = allExpressionsToCloneableNodesMap.begin(); it != allExpressionsToCloneableNodesMap.end(); ++it) {
        allExpressions.insert(it->first);
    }
}

void PREOptimizer::iteratePartialRedundancyElimination()
{
    bool modified = true;
    // while (modified) {
    ///ONCE:
        modified = partialRedundancyEliminationOnce();
    // }
}

bool PREOptimizer::partialRedundancyEliminationOnce()
{
   
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);

  
    //first add new basic blocks to the flowgraph to basic blocks that have more than one predecessor
    addNewBasicBlocksToMultiplePredecessorNodes();

    ///WARNING: might need to redraw the flowgraph???? since some instruction blocks not updated properly... like IfNode to IfElseNode conversion!!! Or fix later
    ///nah boy if i redraw the flowgraph, it will remove the new basic blocks that were added

    //entryBasicBlock is still valid for this augmented flowgraph

    //analysis
    USED used(entryBasicBlock);
    this->nodeLatestExpressionsSets = used.getNodesLatestExpressionsSets();
    this->nodeOutUsedSets = used.getNodeOutDataFlowSets();
  

    //transformation
    bool modified = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        modified |= basicBlockRemovePartialRedundancy(basicBlock);
    }

    //redraw the flowgraph to remove unnecessary basic blocks divisions
    ///HOWEVER: would need to change signature to return this new entryBasicBlock to the main function


    return modified;    //whether any partial redundancy was removed
}



void PREOptimizer::addNewBasicBlocksToMultiplePredecessorNodes()
{
    //iterate through all the basic blocks
    //if a basic block has more than one predecessor, add a new basic block between that node and its predecessor
    //add the new basic block to the flowgraph AND ir tree
    //at the end refetch the basic blocks vector

    for (BasicBlock* basicBlock : basicBlocks)
    {
        if (basicBlock->get_predecessors().size() > 1)
        {
            //go through all the predecessors of the basic block
            //if the predecessor does not end with and EndBlockNode (which means we can directly insert BEFORE that instruction)
            //insert a new basic block between the predecessor and the basic block
            for (BasicBlock* predecessor : basicBlock->get_predecessors())
            {
                //if it is an EndBlockNode, the insertion is more simple, so no need to create a new basic block
                BaseNode* lastInstruction = predecessor->get_instructions_copy().back();
                if (dynamic_cast<EndBlockNode*>(lastInstruction) == nullptr)
                {   
                    //otherwise, check what the last instruction is
                    /// ONE: it is a TestNode. This means we need to convert IfNode --> IfElseNode
                    /// TWO: it is a LoopNode. This means we need to insert a new instruction at the start of the LoopNode body
                    /// THREE: it is a SimpleNode (not TestNode). This means we need to insert a new instruction after the SimpleNode
                    InsertableBasicBlock::NodeInsertionStrategy* insertionStrategy;
                    if (dynamic_cast<TestNode*>(lastInstruction) != nullptr)
                    {
                        insertionStrategy = AnalysisTools::createNewElseBlockInsertionStrategy(dynamic_cast<IfNode*>(lastInstruction));
                    }
                    else if (dynamic_cast<LoopNode*>(lastInstruction) != nullptr)
                    {
                        insertionStrategy = AnalysisTools::createLoopBodyStartInsertionStrategy(dynamic_cast<LoopNode*>(lastInstruction));
                    }
                    else
                    {
                        insertionStrategy = AnalysisTools::createAfterSimpleNodeInsertionStrategy(dynamic_cast<SimpleNode*>(lastInstruction));
                    }
                  

                    //create a new basic block with the insertion strategy
                    InsertableBasicBlock* newBasicBlock = new InsertableBasicBlock(insertionStrategy);

                    //insert the new basic block between the predecessor and the basic block
                    //can call the insertion strategy when a non-PlaceholderNode needs to be inserted to the IR tree
                    basicBlock->insert_sandwich_predecessor_basic_block(predecessor, newBasicBlock);

                    //add the new basic block to the basic blocks vector for PREOptimizer
                    basicBlocks.push_back(newBasicBlock);
                }
            }
        }
    }
}

bool PREOptimizer::basicBlockRemovePartialRedundancy(BasicBlock* basicBlock) {

    bool modified = false;

    //iterate through all the instructions in the basic block
    //if the current instruction generates an expression which is partially redundant, then remove it

    std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end();) {
        BaseNode* instruction = *it;


        ///FIRST: check if you need to add any t=x+y nodes
        //get the temp expressions to add
        std::set<std::string> tempExpressionsToAdd = getTempExpressionsToAdd(instruction);

        //for each temp expression, insert a new expression node in the IR tree and instruction list
        for (std::string tempExpression : tempExpressionsToAdd) {
            modified = true;
            // std::cout << "Adding temp expression: " << tempExpression << std::endl;
            ExpressionNode* newExpressionNode = getNewTempExpressionNodeToAdd(tempExpression);

            //If it is a placeholder node, then we need to use the insertion strategy
            //Otherwise, we can just insert the node above the current instruction
            if (dynamic_cast<PlaceholderNode*>(instruction) != nullptr) {
                InsertableBasicBlock* insertableBasicBlock = dynamic_cast<InsertableBasicBlock*>(basicBlock);
                insertableBasicBlock->executeNodeIRInsertion(it, newExpressionNode);
            }else{
                ///WARNING: actually we want to insert above.....
                // basicBlock->insert_sandwich_instruction_node(it, newExpressionNode, false);
                basicBlock->insert_parent_instruction_node(it, newExpressionNode);
            }
        }

        ///SECOND: check if you need to replace any original z=x+y with the temp variable z=t

        //get the used/referenced expression at this node (should maximum be 1)
        std::set<std::string> usedExpressions = instruction->getReferencedExpressions();
        assert(usedExpressions.size() <= 1);
        if (usedExpressions.size() == 1) {
            std::string expression = *usedExpressions.begin();
            
            if (isExpressionReplacedByTemp(instruction, expression)) {   //then we should place with a mov node
                modified = true;

                std::string tempVar = getOrAddNewTempVariableForExpression(expression);
                MovNode* movNode = new MovNode(expression, tempVar);

                // std::cout <<"Replacing expression: " << expression << "with temp variable" <<tempVar <<std::endl;
             
                it = basicBlock->replace_instruction_node(it, movNode);
                it--;   //go back one to the new node, so can increment uniformly with ++it
                ///WARNING: this deletes the instruction node 'instruction'
            }
        }

        //increment the iterator to the next instruction
        ++it;
    }

    return modified;
}

//because I guess at a particular node, multiple new t=x+y could needed to be added
std::set<std::string> PREOptimizer::getTempExpressionsToAdd(BaseNode* instruction) {
    //latest intersect used.out
    std::set<std::string> latestExpressions = nodeLatestExpressionsSets[instruction];
    std::set<std::string> usedOut = nodeOutUsedSets[instruction];
    std::set<std::string> result;  // Add this
    std::set_intersection(
        latestExpressions.begin(), latestExpressions.end(),
        usedOut.begin(), usedOut.end(),
        std::inserter(result, result.begin())  // Add this
    );
    return result;
}

ExpressionNode* PREOptimizer::getNewTempExpressionNodeToAdd(std::string &expression) {

    std::string tempVar;
    if (expressionToTempVarMap.find(expression) != expressionToTempVarMap.end()) {
        //retrieve the node from the map
        tempVar = expressionToTempVarMap[expression];
    }
    else {
        //create a new temp variable and add it to the map
        tempVar = "_s" + std::to_string(nextProgramTempVariableCount++);
        expressionToTempVarMap[expression] = tempVar;

        //add a declare statement to the start of the program
        DeclareNode* declareTempVarNode = new DeclareNode(tempVar);

        //get the first instruction of the program from the first basic block
        ///WARNING: this assumes the instructions are in normal order, not reversed
        std::list<BaseNode*>::iterator declarationIterator = entryBasicBlock->get_instructions_reference().begin();
        entryBasicBlock->insert_sandwich_instruction_node(declarationIterator, declareTempVarNode, false);
    }

    //now fetch the node from the other map (expressionToCloneableNodesMap)
    ExpressionNode* cloneableNode = allExpressionsToCloneableNodesMap[expression];
    ExpressionNode* newExpressionNode = dynamic_cast<ExpressionNode*>(cloneableNode->copyNodeOnly());
    newExpressionNode->setDest(tempVar);

    return newExpressionNode;
}

std::string PREOptimizer::getOrAddNewTempVariableForExpression(std::string &expression) {
    //if the expression is already in the map, return the temp variable
    if (expressionToTempVarMap.find(expression) != expressionToTempVarMap.end()) {
        return expressionToTempVarMap[expression];
    }

    //increment nextProgramTempVariableCount after creating the new program temp variable
    std::string programTempVar = "_s" + std::to_string(nextProgramTempVariableCount++);

    //add the temp variable to the expressionToTempVarMap
    expressionToTempVarMap[expression] = programTempVar;

    //add a declare statement to the start of the program
    DeclareNode* declareTempVarNode = new DeclareNode(programTempVar);

    //get the first instruction of the program from the first basic block
    ///WARNING: this assumes the instructions are in normal order, not reversed
    std::list<BaseNode*>::iterator declarationIterator = entryBasicBlock->get_instructions_reference().begin();
    entryBasicBlock->insert_sandwich_instruction_node(declarationIterator, declareTempVarNode, false);

    return programTempVar;
}

bool PREOptimizer::isExpressionReplacedByTemp(BaseNode* instruction, std::string &expression) {
    //if the instruction is a MovNode and the expression is the same as the dest, then return true
    std::set<std::string> unionSet;

    std::set<std::string> latestExpressions = nodeLatestExpressionsSets[instruction];
    std::set<std::string> usedOut = nodeOutUsedSets[instruction];

    std::set<std::string> notLatestExpressions;
    std::set_difference(allExpressions.begin(), allExpressions.end(), latestExpressions.begin(), latestExpressions.end(), std::inserter(notLatestExpressions, notLatestExpressions.begin()));

    std::set_union(notLatestExpressions.begin(), notLatestExpressions.end(), usedOut.begin(), usedOut.end(), std::inserter(unionSet, unionSet.begin()));
    
    //find if the expression is in the union set
    return unionSet.find(expression) != unionSet.end();
}

int PREOptimizer::getNextProgramTempVariableCount() {
    return nextProgramTempVariableCount;
}