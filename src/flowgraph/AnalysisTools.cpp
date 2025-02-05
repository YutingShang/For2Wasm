#include "AnalysisTools.h"
#include <set>
#include <vector>
#include <queue>
#include <regex>
#include <iostream>
#include "ExpressionNode.h"
#include "MovNode.h"
#include "VBE.h"
#include "AVAIL_PRE.h"
#include "POST.h"
#include <algorithm>
#include "InsertableBasicBlock.h"
#include "LoopNode.h"
#include "IfNode.h"
#include "IfElseNode.h"

std::vector<std::shared_ptr<BasicBlock>> AnalysisTools::getBasicBlocks(std::shared_ptr<BasicBlock> entryBasicBlock) {
    std::vector<std::shared_ptr<BasicBlock>> basicBlocks;

    // get all the basic blocks in the flowgraph by BFS, and output into the basicBlocks vector
    std::queue<std::shared_ptr<BasicBlock>> toExploreQueue;
    toExploreQueue.push(entryBasicBlock);
    std::set<std::shared_ptr<BasicBlock>> seen;
    seen.insert(entryBasicBlock);

    while (!toExploreQueue.empty()) {
        std::shared_ptr<BasicBlock> current = toExploreQueue.front();
        basicBlocks.push_back(current); // visit the current basic block
        toExploreQueue.pop();

        // add all the successors to the queue if they are not already seen
        for (std::shared_ptr<BasicBlock> successor : current->get_successors()) {
            if (seen.find(successor) == seen.end()) { // check 'seen' set
                toExploreQueue.push(successor);
                seen.insert(successor);
            }
        }
    }

    return basicBlocks;
}

std::set<std::string> AnalysisTools::getAllProgramExpressions(std::shared_ptr<BaseNode> entryNode) {
    std::set<std::string> allExpressions;

    //IR tree traversal (not a graph)
    std::queue<std::shared_ptr<BaseNode>> toExploreQueue;
    toExploreQueue.push(entryNode);
    while (!toExploreQueue.empty()) {
        std::shared_ptr<BaseNode> current = toExploreQueue.front();
        toExploreQueue.pop();
        //get generated expressions, insert into allExpressions set
        const std::set<std::string>& generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(current);
        allExpressions.insert(generatedExpressions.begin(), generatedExpressions.end());   //copy all set of all generated expressions into allExpressions
        //add children to the queue
        for (std::shared_ptr<BaseNode> child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allExpressions;
}

std::unordered_map<std::string, std::shared_ptr<ExpressionNode>> AnalysisTools::getAllProgramExpressionsToCloneableNodesMap(std::shared_ptr<BaseNode> entryNode) {
    std::unordered_map<std::string, std::shared_ptr<ExpressionNode>> allExpressionsToCloneableNodesMap;  

    //IR tree traversal (not a graph)
    std::queue<std::shared_ptr<BaseNode>> toExploreQueue;
    toExploreQueue.push(entryNode);
    while (!toExploreQueue.empty()) {
        std::shared_ptr<BaseNode> current = toExploreQueue.front();
        toExploreQueue.pop();
        //get generated expressions, insert into allExpressions set
        const std::set<std::string>& generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(current);
        for (const auto &expression : generatedExpressions) {
            if (allExpressionsToCloneableNodesMap.find(expression) == allExpressionsToCloneableNodesMap.end()) {   //if expression is not already in the map
                allExpressionsToCloneableNodesMap[expression] = std::dynamic_pointer_cast<ExpressionNode>(current.get()->cloneContent());   //add the expression to the map, with the corresponding prototype node
            }
        }
        //add children to the queue
        for (std::shared_ptr<BaseNode> child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allExpressionsToCloneableNodesMap;
}

std::set<std::string> AnalysisTools::getKilledExpressionsAtNode(std::shared_ptr<BaseNode> node, std::set<std::string> &allExpressions) {
    std::set<std::string> killedExpressions;

    //get the defined variables of the node, e.g. {x}
    std::set<std::string> definedVariables = node->getDefinedVariables();

    //Then all E_x expressions (which contains the variable x) are killed by the node
    for (const auto &programExpr : allExpressions) {

        //split the expression based on symbols (+, -, *, /, |, &, !, <, > , <=, >=, ==)
        std::regex symbolRegex("([+\\-*/|&!<>=]+)");
        std::sregex_token_iterator exprIter(programExpr.begin(), programExpr.end(), symbolRegex, -1);     //split the expression based on symbols
        std::sregex_token_iterator exprEnd;      //default end iterator

        //check if the expression contains any of the defined variables
        bool containsDefinedVariable = false;
        while (exprIter != exprEnd && !containsDefinedVariable) {
            std::string exprToken = *exprIter;
            if (definedVariables.find(exprToken) != definedVariables.end()) {
                containsDefinedVariable = true;
            }
            ++exprIter;
        }
        if (containsDefinedVariable) {        //e.g. expression contains x, so it is killed by the node
            killedExpressions.insert(programExpr);
        }
    }
  
    return killedExpressions;
} 

// std::unordered_map<std::string, std::set<BaseNode*>> AnalysisTools::getAllProgramDefinitionPoints(BaseNode* entryNode) {
//     std::unordered_map<std::string, std::set<BaseNode*>> allDefinitionPoints;

//     //IR tree traversal (not a graph)
//     std::queue<BaseNode*> toExploreQueue;
//     toExploreQueue.push(entryNode);
//     while (!toExploreQueue.empty()) {
//         //for each IR node
//         BaseNode* current = toExploreQueue.front();
//         toExploreQueue.pop();

//         //get generated expressions
//         // const std::set<std::string>& generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(current);

//         //get defined variables
//         std::set<std::string> definedVariables = current->getDefinedVariables();
//         if (!definedVariables.empty()) {  //if the node defines a variable

//             assert(definedVariables.size() == 1);     //assume only 1 variable is defined 
//             std::string definedVariable = *definedVariables.begin();

//             // for (const auto &generatedExpression : generatedExpressions) {
//             allDefinitionPoints[definedVariable].insert(current);    //add expressions to set of definitions for defined_variable
//             // }
//         }

//         //add children of the IR node to the queue
//         for (BaseNode* child : current->getChildren()) {
//             toExploreQueue.push(child);
//         }
//     }
//     return allDefinitionPoints;
// }

std::set<std::string> AnalysisTools::getDefinitionsAtNode(std::shared_ptr<BaseNode> node) {
    std::set<std::string> definitions;
    if (std::dynamic_pointer_cast<ExpressionNode>(node)) {
        definitions = AnalysisTools::getGeneratedExpressionsAtNode(node);
    } else if (std::dynamic_pointer_cast<MovNode>(node)) {
        std::shared_ptr<MovNode> movNode = std::dynamic_pointer_cast<MovNode>(node);
        definitions = {movNode->getSrc()};
    }
    return definitions;
}

std::set<std::pair<std::string, std::string>> AnalysisTools::getAllProgramCopyStatements(std::shared_ptr<BaseNode> entryNode) {
    std::set<std::pair<std::string, std::string>> allCopyStatements;
    //traverse the IR tree, find all MovNodes and add their src and dst to the set
    std::queue<std::shared_ptr<BaseNode>> toExploreQueue;
    toExploreQueue.push(entryNode);
    while (!toExploreQueue.empty()) {
        std::shared_ptr<BaseNode> current = toExploreQueue.front();
        toExploreQueue.pop();

        if (std::dynamic_pointer_cast<MovNode>(current)) {
            std::shared_ptr<MovNode> movNode = std::dynamic_pointer_cast<MovNode>(current);
            allCopyStatements.insert(std::make_pair(movNode->getDest(), movNode->getSrc()));
        }

        //add children of the IR node to the queue
        for (std::shared_ptr<BaseNode> child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allCopyStatements;
}

std::set<std::pair<std::string, std::string>> AnalysisTools::getKilledCopyStatementsAtNode(std::shared_ptr<BaseNode> node, std::set<std::pair<std::string, std::string>> &allCopyStatements) {
    std::set<std::pair<std::string, std::string>> killedCopyStatements;

    //get the defined variables of the node, e.g. {x}
    std::set<std::string> definedVariables = node->getDefinedVariables();

    //for every defined variable x, it kills/invalidates all copy statements (x, _) and (_, x)
    for (const auto &definedVariable : definedVariables) {
        for (const auto &copyStatement : allCopyStatements) {
            if (copyStatement.first == definedVariable || copyStatement.second == definedVariable) {
                killedCopyStatements.insert(copyStatement);
            }
        }
    }

    return killedCopyStatements;
}

std::set<std::pair<std::string, std::string>> AnalysisTools::getGeneratedCopyStatementsAtNode(std::shared_ptr<BaseNode> node) {
    std::set<std::pair<std::string, std::string>> generatedCopyStatements;
    if (std::dynamic_pointer_cast<MovNode>(node)) {
        std::shared_ptr<MovNode> movNode = std::dynamic_pointer_cast<MovNode>(node);
        generatedCopyStatements.insert(std::make_pair(movNode->getDest(), movNode->getSrc()));
    }
    return generatedCopyStatements;
}

std::set<std::string> AnalysisTools::getGeneratedExpressionsAtNode(std::shared_ptr<BaseNode> node) {
    //e.g. if x = x+y, then we say x+y has NOT been generated by this node (even though it is used/referenced)
    //generated expressions = referenced expressions - killed expressions


    std::set<std::string> generatedExpressions;
    std::set<std::string> referencedExpressions = node->getReferencedExpressions();

    //get the defined variables of the node, e.g. {x}
    std::set<std::string> definedVariables = node->getDefinedVariables();

    for (const auto &programExpr : referencedExpressions) {

        //split the expression based on symbols (+, -, *, /, |, &, !, <, > , <=, >=, ==)
        std::regex symbolRegex("([+\\-*/|&!<>=]+)");
        std::sregex_token_iterator exprIter(programExpr.begin(), programExpr.end(), symbolRegex, -1);     //split the expression based on symbols
        std::sregex_token_iterator exprEnd;      //default end iterator

        //check if the expression contains any of the defined variables
        bool containsDefinedVariable = false;
        while (exprIter != exprEnd && !containsDefinedVariable) {
            std::string exprToken = *exprIter;
            if (definedVariables.find(exprToken) != definedVariables.end()) {
                containsDefinedVariable = true;
            }
            ++exprIter;
        }
        if (!containsDefinedVariable) {        //e.g. expression does not contain x, so it is generated by the node
            generatedExpressions.insert(programExpr);
        }
    }
    return generatedExpressions;
}


std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> AnalysisTools::getAllNodesEarliestExpressions(std::shared_ptr<BasicBlock> entryBasicBlock) {
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesEarliestExpressions;

    //get the anticipated[B].in expressions of all nodes
    VBE vbe(entryBasicBlock);
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesAnticipatedExpressions = vbe.getNodeInDataFlowSets();

    //get the available[B].in expressions of all nodes
    AVAIL_PRE avail_pre(entryBasicBlock);
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesAvailableExpressions = avail_pre.getNodeInDataFlowSets();

    //For each node: earliest[B] = anticipated[B].in - available[B].in
    for (auto& [node, anticipatedExpressions] : allNodesAnticipatedExpressions) {
        std::set<std::string> earliestExpressions;
        std::set<std::string> availableExpressions = allNodesAvailableExpressions[node];
        std::set_difference(anticipatedExpressions.begin(), anticipatedExpressions.end(), availableExpressions.begin(), availableExpressions.end(), std::inserter(earliestExpressions, earliestExpressions.begin()));
        allNodesEarliestExpressions[node] = earliestExpressions;
    }

    return allNodesEarliestExpressions;
}

std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> AnalysisTools::getAllNodesLatestExpressions(std::shared_ptr<BasicBlock> entryBasicBlock, std::set<std::string> allExpressions, std::vector<std::shared_ptr<BasicBlock>> basicBlocks) {
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesLatestExpressions;

    //get the earliest expressions of all nodes
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesEarliestExpressions = getAllNodesEarliestExpressions(entryBasicBlock);

    //get the in-POST expressions of all nodes
    POST post(entryBasicBlock);
    std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesInPostExpressions = post.getNodeInDataFlowSets();

    //get node:basicBlock map for all nodes in order to getSuccessorNodes faster
    std::map<std::weak_ptr<BaseNode>, std::shared_ptr<BasicBlock>, std::owner_less<std::weak_ptr<BaseNode>>> flowgraphNodeToBasicBlockMap = getFlowgraphNodeToBasicBlockMap(basicBlocks);


    //latest[B] = (earliest[B] ∪ in-POST[B]) ∩ (e_useB ∪ ¬(∩ successors S: (earliest[S] ∪ in-POST[S]))
    for (auto& [node, earliestExpressions] : allNodesEarliestExpressions) {     //FOR EACH NODE, find the latest expression set

        if (node.expired()) {
            throw std::runtime_error("Node has expired, cannot get latest expressions");
}

        //first get (earliest[B] ∪ in-POST[B])
        std::set<std::string> inPostExpressions = allNodesInPostExpressions[node];
        std::set<std::string> earliestOrInPostExpressions;
        std::set_union(earliestExpressions.begin(), earliestExpressions.end(), inPostExpressions.begin(), inPostExpressions.end(), std::inserter(earliestOrInPostExpressions, earliestOrInPostExpressions.begin()));

        //get the used/referenced expressions at the node
        std::set<std::string> usedExpressions = node.lock()->getReferencedExpressions();

        //get the successors of the node
        std::shared_ptr<BasicBlock> currentBasicBlock = flowgraphNodeToBasicBlockMap[node];
        std::vector<std::weak_ptr<BaseNode>> successorNodes = getSuccessorNodes(node.lock(), currentBasicBlock);

        //intersect over all successors
        std::set<std::string> intersectionOfSuccessorEarliestOrInPostExpressions = allExpressions;
        //for every successor node, find (earliest[S] ∪ in-POST[S])
        for (std::weak_ptr<BaseNode> successor : successorNodes) {
            std::set<std::string> successorEarliestExpressions = allNodesEarliestExpressions[successor];
            std::set<std::string> successorInPostExpressions = allNodesInPostExpressions[successor];
            std::set<std::string> earliestOrInPostExpressions;     //for this successor node, find the union of earliest and postponable expressions
            std::set_union(successorEarliestExpressions.begin(), successorEarliestExpressions.end(), successorInPostExpressions.begin(), successorInPostExpressions.end(), std::inserter(earliestOrInPostExpressions, earliestOrInPostExpressions.begin()));

            //intersect the next successor with the current intersection
            std::set<std::string> tempIntersection;
            std::set_intersection(intersectionOfSuccessorEarliestOrInPostExpressions.begin(), intersectionOfSuccessorEarliestOrInPostExpressions.end(), earliestOrInPostExpressions.begin(), earliestOrInPostExpressions.end(), std::inserter(tempIntersection, tempIntersection.begin()));
            intersectionOfSuccessorEarliestOrInPostExpressions = tempIntersection;

            }

        //get the complementation of the intersection of all successors
        std::set<std::string> complementationOfIntersectionOfSuccessors;
        std::set_difference(allExpressions.begin(), allExpressions.end(), intersectionOfSuccessorEarliestOrInPostExpressions.begin(), intersectionOfSuccessorEarliestOrInPostExpressions.end(), std::inserter(complementationOfIntersectionOfSuccessors, complementationOfIntersectionOfSuccessors.begin()));

        //finally, find the intersection of the earliest or in-post expressions and the complementation of the intersection of all successors
        std::set<std::string> latestExpressions;
        std::set_intersection(earliestOrInPostExpressions.begin(), earliestOrInPostExpressions.end(), complementationOfIntersectionOfSuccessors.begin(), complementationOfIntersectionOfSuccessors.end(), std::inserter(latestExpressions, latestExpressions.begin()));

        //store the latest expression set for the node
        allNodesLatestExpressions[node] = latestExpressions;
    }

    return allNodesLatestExpressions;
}


std::map<std::weak_ptr<BaseNode>, std::shared_ptr<BasicBlock>, std::owner_less<std::weak_ptr<BaseNode>>> AnalysisTools::getFlowgraphNodeToBasicBlockMap(std::vector<std::shared_ptr<BasicBlock>> basicBlocks) {
    std::map<std::weak_ptr<BaseNode>, std::shared_ptr<BasicBlock>, std::owner_less<std::weak_ptr<BaseNode>>> flowgraphNodeToBasicBlockMap;

    for (std::shared_ptr<BasicBlock> basicBlock : basicBlocks) {
        for (std::weak_ptr<BaseNode> node : basicBlock->get_instructions_copy()) {
            flowgraphNodeToBasicBlockMap[node] = basicBlock;
        }
    }
    return flowgraphNodeToBasicBlockMap;
}


std::vector<std::weak_ptr<BaseNode>> AnalysisTools::getSuccessorNodes(std::shared_ptr<BaseNode> node, std::shared_ptr<BasicBlock> currentBasicBlock) {
    std::vector<std::weak_ptr<BaseNode>> successorNodes;

    std::list<std::weak_ptr<BaseNode>> instructions = currentBasicBlock->get_instructions_copy();

    //if its not the last node of the basic block, then the instruction after the current node in the instructions list is the successor
    //otherwise, get the successor basic blocks of the current basic block, and take the first instruction of each successor basic block
    if (node != instructions.back().lock()) {
        std::list<std::weak_ptr<BaseNode>>::const_iterator currentNodeIterator = std::find_if(instructions.begin(), instructions.end(), [node](const std::weak_ptr<BaseNode>& listElt){return listElt.lock() == node;});
        if (currentNodeIterator != instructions.end()) {
            auto nextIterator = std::next(currentNodeIterator);
            if (nextIterator != instructions.end()) {
                successorNodes.push_back(*nextIterator);
        }
    }
    } else {
        //get the successor basic blocks of the current basic block
        std::vector<std::shared_ptr<BasicBlock>> successorBasicBlocks = currentBasicBlock->get_successors();
        //get the first instruction of each successor basic block
        for (std::shared_ptr<BasicBlock> successorBasicBlock : successorBasicBlocks) {
            successorNodes.push_back(successorBasicBlock->get_instructions_copy().front());
        }
    }
    return successorNodes;
}

/////////////////////////////////////////////////////////
/////////////////////FACTORY METHODS/////////////////////
/////////////////////////////////////////////////////////


std::unique_ptr<InsertableBasicBlock::NodeInsertionStrategy> AnalysisTools::createLoopBodyStartInsertionStrategy(std::shared_ptr<LoopNode> loopNodeToInsertAfter) {

    class LoopBodyStartInserter : public InsertableBasicBlock::NodeInsertionStrategy {
        std::shared_ptr<LoopNode> loopNodeToInsertAfter;

        public:
            //explicit constructor
            explicit LoopBodyStartInserter(std::shared_ptr<LoopNode> loopNodeToInsertAfter)
                : loopNodeToInsertAfter(loopNodeToInsertAfter) {}

            void firstInsertionStrategy(std::shared_ptr<SimpleNode> nodeToInsert) override {
                // Insert after the loopNodeToInsertAfter
                loopNodeToInsertAfter->insertSandwichBodyChild(nodeToInsert);
            }
    };

    return std::make_unique<LoopBodyStartInserter>(loopNodeToInsertAfter);
}


std::unique_ptr<InsertableBasicBlock::NodeInsertionStrategy> AnalysisTools::createAfterSimpleNodeInsertionStrategy(std::shared_ptr<SimpleNode> simpleNodeToInsertAfter) {
    
    class AfterSimpleNodeInserter : public InsertableBasicBlock::NodeInsertionStrategy {
        std::shared_ptr<SimpleNode> simpleNodeToInsertAfter;

        public:
            explicit AfterSimpleNodeInserter(std::shared_ptr<SimpleNode> simpleNodeToInsertAfter)
                : simpleNodeToInsertAfter(simpleNodeToInsertAfter) {}

            void firstInsertionStrategy(std::shared_ptr<SimpleNode> nodeToInsert) override {
                simpleNodeToInsertAfter->insertSandwichChild(nodeToInsert);
            }
    };

    return std::make_unique<AfterSimpleNodeInserter>(simpleNodeToInsertAfter);
}

std::unique_ptr<InsertableBasicBlock::NodeInsertionStrategy> AnalysisTools::createNewElseBlockInsertionStrategy(std::shared_ptr<IfNode> ifNodeToInsertAfter) {
    
    class NewElseBlockInserter : public InsertableBasicBlock::NodeInsertionStrategy {
        std::shared_ptr<IfNode> ifNodeToInsertAfter;

        public:
            explicit NewElseBlockInserter(std::shared_ptr<IfNode> ifNodeToInsertAfter)
                : ifNodeToInsertAfter(ifNodeToInsertAfter) {}

            void firstInsertionStrategy(std::shared_ptr<SimpleNode> nodeToInsert) override {
                //now that we know we want to insert this nodeToInsert
                
                ///FIRST: convert the ifNode to an ifElseNode
                ///ISSUE: need to change the instructions list pointer - maybe just convert the node implicity??
                // std::unique_ptr<IfElseNode> ifElseNode = ifNodeToInsertAfter->convertToIfElseNode();
                std::shared_ptr<IfElseNode> ifElseNode = ifNodeToInsertAfter->convertToIfElseNode();

                ///SECOND: add the nodeToInsert to the ifElseNode BEFORE the ENDELSE node of the else block
                std::shared_ptr<BaseNode> endElseNode = ifElseNode->getElseNode();
                assert(std::dynamic_pointer_cast<EndBlockNode>(endElseNode));     //check it should be an EndBlockNode for ENDELSE
                endElseNode->insertSandwichParent(nodeToInsert);

            }
    };

    return std::make_unique<NewElseBlockInserter>(ifNodeToInsertAfter);
}