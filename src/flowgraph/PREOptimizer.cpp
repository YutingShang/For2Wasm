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
#include "EntryNode.h"
#include "PropagationOptimizer.h"


PREOptimizer::PREOptimizer(std::shared_ptr<EntryNode> entryNode, std::unordered_map<std::string, std::string> irDatatypeMap, int nextProgramTempVariableCount) : entryNode(entryNode), irDatatypeMap(irDatatypeMap), nextProgramTempVariableCount(nextProgramTempVariableCount) {   
}

std::unordered_map<std::string, std::string> PREOptimizer::getUpdatedIRDatatypeMap() const {
    return irDatatypeMap;
}

bool PREOptimizer::runPartialRedundancyElimination()
{
    //reinitialise expressions map each time we run PRE. IR Tree has changed 
    this->allExpressionsToCloneableNodesMap = AnalysisTools::getAllProgramExpressionsToCloneableNodesMap(std::static_pointer_cast<BaseNode>(entryNode));
    //get all expressions in the program, just take the keys from the map instead of traversing the IR tree again
    for (auto it = allExpressionsToCloneableNodesMap.begin(); it != allExpressionsToCloneableNodesMap.end(); ++it) {
        allExpressions.insert(it->first);
    }

    //reinitialise the irExpressionToDatatypeMap
    irExpressionToDatatypeMap = AnalysisTools::getAllProgramExpressionsToIRDatatypeMap(entryNode, irDatatypeMap);

    //redraw flowgraph to synchronize with the updated IR tree
    startBasicBlock = AnalysisTools::drawFlowgraph(entryNode);

    //need to first get the basic blocks, then add new basic blocks to the flowgraph
    basicBlocks = AnalysisTools::getBasicBlocks(startBasicBlock);
    //add new basic blocks to the flowgraph to basic blocks that have more than one predecessor
    addNewBasicBlocksToMultiplePredecessorNodes();
    

    // Analysis: VBE, AVAIL_PRE, POST, USED
    USED used(startBasicBlock);
    this->nodeLatestExpressionsSets = used.getNodesLatestExpressionsSets();
    this->nodeOutUsedSets = used.getNodeOutDataFlowSets();           

    // Transformation: Remove partial redundancy
    bool modified = false;
    for (std::shared_ptr<BasicBlock> basicBlock : basicBlocks)
    {
        modified |= basicBlockRemovePartialRedundancy(basicBlock);
    }

    return modified;    //whether any partial redundancy was removed
}

bool PREOptimizer::runPartialRedundancyEliminationEfficiently()
{
        //reinitialise expressions map each time we run PRE. IR Tree has changed 
    this->allExpressionsToCloneableNodesMap = AnalysisTools::getAllProgramExpressionsToCloneableNodesMap(std::static_pointer_cast<BaseNode>(entryNode));
    //get all expressions in the program, just take the keys from the map instead of traversing the IR tree again
    for (auto it = allExpressionsToCloneableNodesMap.begin(); it != allExpressionsToCloneableNodesMap.end(); ++it) {
        allExpressions.insert(it->first);
    }

    //reinitialise the irExpressionToDatatypeMap
    irExpressionToDatatypeMap = AnalysisTools::getAllProgramExpressionsToIRDatatypeMap(entryNode, irDatatypeMap);

    //redraw flowgraph to synchronize with the updated IR tree
    startBasicBlock = AnalysisTools::drawFlowgraph(entryNode);

    //need to first get the basic blocks, then add new basic blocks to the flowgraph
    basicBlocks = AnalysisTools::getBasicBlocks(startBasicBlock);
    //add new basic blocks to the flowgraph to basic blocks that have more than one predecessor
    addNewBasicBlocksToMultiplePredecessorNodes();
    

    // Analysis: VBE, AVAIL_PRE, POST, USED
    VBE vbe(startBasicBlock);
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesAnticipatedInExpressions = vbe.getNodeInDataFlowSets();

    AVAIL_PRE avail_pre(startBasicBlock, allNodesAnticipatedInExpressions);
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesAvailableExpressions = avail_pre.getNodeInDataFlowSets();

    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesEarliestExpressions = AnalysisTools::getAllNodesEarliestExpressions(startBasicBlock, allNodesAnticipatedInExpressions, allNodesAvailableExpressions);

    POST post(startBasicBlock, allNodesEarliestExpressions);
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesInPostExpressions = post.getNodeInDataFlowSets();

    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesLatestExpressions = AnalysisTools::getAllNodesLatestExpressions(startBasicBlock, allExpressions, basicBlocks, std::optional{allNodesEarliestExpressions}, std::optional{allNodesInPostExpressions});

    USED used(startBasicBlock, allNodesLatestExpressions);
    this->nodeLatestExpressionsSets = allNodesLatestExpressions;
    this->nodeOutUsedSets = used.getNodeOutDataFlowSets();

    // Transformation: Remove partial redundancy
    bool modified = false;
    for (std::shared_ptr<BasicBlock> basicBlock : basicBlocks)
    {
        modified |= basicBlockRemovePartialRedundancy(basicBlock);
    }

    return modified;    //whether any partial redundancy was removed
}

bool PREOptimizer::iteratePRE_CopyPropagation()
{
    bool modified = true;
    while (modified) {
        modified = runPartialRedundancyElimination();
        PropagationOptimizer propagationOptimizer(entryNode);
        modified |= propagationOptimizer.runCopyPropagation();
    }
    return modified;
}  

bool PREOptimizer::iterateEfficientPRE_CopyPropagation()
{
    bool modified = true;
    while (modified) {
        modified = runPartialRedundancyEliminationEfficiently();
        PropagationOptimizer propagationOptimizer(entryNode);
        modified |= propagationOptimizer.runCopyPropagation();
    }
    return modified;
}


void PREOptimizer::addNewBasicBlocksToMultiplePredecessorNodes()
{
    //iterate through all the basic blocks
    //if a basic block has more than one predecessor, add a new basic block between that node and its predecessor
    //add the new basic block to the flowgraph AND ir tree
    //at the end refetch the basic blocks vector

    std::vector<std::shared_ptr<BasicBlock>> originalBasicBlocks = basicBlocks;   //make a copy since we add new basic blocks to the flowgraph in the loop

    for (std::shared_ptr<BasicBlock> basicBlock : originalBasicBlocks)
    {
        std::vector<std::weak_ptr<BasicBlock>> predecessors = basicBlock->get_predecessors();
        if (predecessors.size() > 1)
        {
           
            //go through all the predecessors of the basic block
            //if the predecessor does not end with and EndBlockNode (which means we can directly insert BEFORE that instruction)
            //insert a new basic block between the predecessor and the basic block
            for (std::weak_ptr<BasicBlock> predecessor_weak_ptr : predecessors)
            {
                std::shared_ptr<BasicBlock> predecessor = predecessor_weak_ptr.lock();
                if (predecessor == nullptr) {
                    throw std::runtime_error("ERROR while handling PRE: predecessor is nullptr");
                }
                {
                    //if it is an EndBlockNode or (do..exit) LoopNode, the insertion is more simple 
                    //(just insert before that instruction - like breaking the endblock/loop node as the new basic block), so no need to create a new basic block
                    std::shared_ptr<BaseNode> lastInstruction = predecessor->get_instructions_copy().back().lock();
                    if (std::dynamic_pointer_cast<EndBlockNode>(lastInstruction) == nullptr && std::dynamic_pointer_cast<LoopNode>(lastInstruction) == nullptr)
                    {   
                        //otherwise, check what the last instruction is
                        /// ONE: it is a TestNode. This means we need to convert IfNode --> IfElseNode (if Test node is part of an IfNode, not a LoopCondNode)
                        /// TWO: it is a SimpleNode like in LoopCondNode init and step (not TestNode). This means we need to insert a new instruction after the SimpleNode
                        std::unique_ptr<InsertableBasicBlock::NodeInsertionStrategy> insertionStrategy;
                        if (std::dynamic_pointer_cast<TestNode>(lastInstruction) != nullptr)
                        {
                            //go backwards from the test node to find the if node
                            std::shared_ptr<IfNode> ifNode;
                            std::list<std::weak_ptr<BaseNode>> bbNodes = predecessor->get_instructions_copy();
                            std::reverse(bbNodes.begin(), bbNodes.end());
                            for (std::weak_ptr<BaseNode> node : bbNodes) {       //search the basic block (ending with the Test node) backwards for an IfNode
                                std::shared_ptr<BaseNode> node_shared = node.lock();
                                if (std::dynamic_pointer_cast<IfNode>(node_shared) != nullptr) {     //found the if node
                                    ifNode = std::dynamic_pointer_cast<IfNode>(node_shared);
                                    break;
                                }
                            }
                            if (ifNode != nullptr) {
                                insertionStrategy = std::move(AnalysisTools::createNewElseBlockInsertionStrategy(ifNode));
                            } else {
                                //otherwise, did not find an if node, it is actually a LoopCondNode 
                                ///NOTE: we are ignoring this case (of exiting from a LoopCondNode, and giving multiple predecessors) for now
                                continue;
                            }
                        }
                        else
                        {
                            insertionStrategy = std::move(AnalysisTools::createAfterSimpleNodeInsertionStrategy(std::dynamic_pointer_cast<SimpleNode>(lastInstruction)));
                        }
                      

                        //create a new basic block with the insertion strategy
                        std::shared_ptr<InsertableBasicBlock> newBasicBlock = std::make_shared<InsertableBasicBlock>(std::move(insertionStrategy));

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
}



bool PREOptimizer::basicBlockRemovePartialRedundancy(const std::shared_ptr<BasicBlock>& basicBlock) {

    bool modified = false;

    //iterate through all the instructions in the basic block
    //if the current instruction generates an expression which is partially redundant, then remove it

    std::list<std::weak_ptr<BaseNode>> instructions = basicBlock->get_instructions_copy();
    for (auto it = instructions.begin(); it != instructions.end();) {
        std::shared_ptr<BaseNode> instruction = it->lock();


        ///FIRST: check if you need to add any t=x+y nodes
        //get the temp expressions to add
        std::set<std::string> tempExpressionsToAdd = getTempExpressionsToAdd(instruction);

        //for each temp expression, insert a new expression node in the IR tree and instruction list
        for (std::string tempExpression : tempExpressionsToAdd) {
            modified = true;
            std::shared_ptr<ExpressionNode> newExpressionNode = getNewTempExpressionNodeToAdd(tempExpression);

            //If it is a placeholder node, then we need to use the insertion strategy
            //Otherwise, we can just insert the node above the current instruction
            if (std::dynamic_pointer_cast<PlaceholderNode>(instruction) != nullptr) {
                std::shared_ptr<InsertableBasicBlock> insertableBasicBlock = std::dynamic_pointer_cast<InsertableBasicBlock>(basicBlock);
                insertableBasicBlock->executeNodeIRInsertion(newExpressionNode);
            }else{
                //e.g. if just an end block node, loop node or other simple node, then insert above the current instruction
                instruction->insertSandwichParent(newExpressionNode);
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

                std::shared_ptr<ExpressionNode> expressionNode = std::dynamic_pointer_cast<ExpressionNode>(instruction);
                if (expressionNode == nullptr) {
                    throw std::runtime_error("ERROR while handling PRE: Instruction is not an ExpressionNode subclass, node: " + instruction->getText());
                }
                std::string originalDest = expressionNode->getDest();
                std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(originalDest, tempVar);

                std::shared_ptr<SimpleNode> simpleNode = std::dynamic_pointer_cast<SimpleNode>(instruction);
                if (simpleNode == nullptr) {
                    throw std::runtime_error("ERROR while handling PRE: Instruction is not a SimpleNode subclass, node: " + instruction->getText());
                }
                simpleNode->replaceCurrentNodeInIRTree(movNode);
            }
        }

        //increment the iterator to the next instruction
        ++it;
       
    }
    return modified;
}

//because I guess at a particular node, multiple new t=x+y could needed to be added
std::set<std::string> PREOptimizer::getTempExpressionsToAdd(std::shared_ptr<BaseNode> instruction) {
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

std::shared_ptr<ExpressionNode> PREOptimizer::getNewTempExpressionNodeToAdd(std::string &expression) {
    std::string IRdatatype = irExpressionToDatatypeMap[expression];

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
        std::shared_ptr<DeclareNode> declareTempVarNode = std::make_shared<DeclareNode>(IRdatatype, tempVar);

        //add the new variable to the irDatatypeMap
        irDatatypeMap[tempVar] = IRdatatype;

        //insert the declare node at the start of the program after the entry node
        entryNode->insertSandwichChild(declareTempVarNode);
    }

    //now fetch the node from the other map (expressionToCloneableNodesMap)
    std::shared_ptr<ExpressionNode>& cloneableNode = allExpressionsToCloneableNodesMap[expression];
    if (cloneableNode == nullptr) {
        throw std::runtime_error("ERROR while handling PRE: Cloneable node not found for expression: " + expression);
    }
    std::shared_ptr<ExpressionNode> newExpressionNode = std::dynamic_pointer_cast<ExpressionNode>(cloneableNode->cloneContent());
    if (newExpressionNode == nullptr) {
        throw std::runtime_error("ERROR while handling PRE: node is not an ExpressionNode subclass: " + expression);
    }
    newExpressionNode->setDest(tempVar);

    return newExpressionNode;
}

std::string PREOptimizer::getOrAddNewTempVariableForExpression(std::string &expression) {
    std::string IRdatatype = irExpressionToDatatypeMap[expression];

    //if the expression is already in the map, return the temp variable
    if (expressionToTempVarMap.find(expression) != expressionToTempVarMap.end()) {
        return expressionToTempVarMap[expression];
    }

    //increment nextProgramTempVariableCount after creating the new program temp variable
    std::string programTempVar = "_s" + std::to_string(nextProgramTempVariableCount++);

    //add the temp variable to the expressionToTempVarMap
    expressionToTempVarMap[expression] = programTempVar;

    //add a declare statement to the start of the program
    std::shared_ptr<DeclareNode> declareTempVarNode = std::make_shared<DeclareNode>(IRdatatype, programTempVar);

    //add the new variable to the irDatatypeMap
    irDatatypeMap[programTempVar] = IRdatatype;

    //insert the declare node at the start of the program after the entry node
    //either adding declaration in transformation b) add t=x+y or c) replace x+y with t
    entryNode->insertSandwichChild(declareTempVarNode);

    return programTempVar;
}

bool PREOptimizer::isExpressionReplacedByTemp(std::shared_ptr<BaseNode> instruction, std::string &expression) {
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

int PREOptimizer::getNextProgramTempVariableCount() const{
    return nextProgramTempVariableCount;
}