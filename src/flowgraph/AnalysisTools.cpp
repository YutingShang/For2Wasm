#include "AnalysisTools.h"
#include <set>
#include <vector>
#include <queue>
#include <regex>
#include <iostream>
#include "ExpressionNode.h"
#include "MovNode.h"
#include "VBE.h"
#include "AVAIL.h"
#include "POST.h"
#include <algorithm>


std::vector<BasicBlock*> AnalysisTools::getBasicBlocks(BasicBlock* entryBasicBlock) {
    std::vector<BasicBlock*> basicBlocks;

    // get all the basic blocks in the flowgraph by BFS, and output into the basicBlocks vector
    std::queue<BasicBlock*> toExploreQueue;
    toExploreQueue.push(entryBasicBlock);
    std::set<BasicBlock*> seen;
    seen.insert(entryBasicBlock);

    while (!toExploreQueue.empty()) {
        BasicBlock* current = toExploreQueue.front();
        basicBlocks.push_back(current); // visit the current basic block
        toExploreQueue.pop();

        // add all the successors to the queue if they are not already seen
        for (BasicBlock* successor : current->get_successors()) {
            if (seen.find(successor) == seen.end()) { // check 'seen' set
                toExploreQueue.push(successor);
                seen.insert(successor);
            }
        }
    }

    return basicBlocks;
}

std::set<std::string> AnalysisTools::getAllProgramExpressions(BaseNode* entryNode) {
    std::set<std::string> allExpressions;

    //IR tree traversal (not a graph)
    std::queue<BaseNode*> toExploreQueue;
    toExploreQueue.push(entryNode);
    while (!toExploreQueue.empty()) {
        BaseNode* current = toExploreQueue.front();
        toExploreQueue.pop();
        //get generated expressions, insert into allExpressions set
        const std::set<std::string>& generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(current);
        allExpressions.insert(generatedExpressions.begin(), generatedExpressions.end());   //copy all set of all generated expressions into allExpressions
        //add children to the queue
        for (BaseNode* child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allExpressions;
}

std::set<std::string> AnalysisTools::getKilledExpressionsAtNode(BaseNode* node, std::set<std::string> &allExpressions) {
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

std::set<std::string> AnalysisTools::getDefinitionsAtNode(BaseNode* node) {
    std::set<std::string> definitions;
    if (dynamic_cast<ExpressionNode*>(node)) {
        definitions = AnalysisTools::getGeneratedExpressionsAtNode(node);
    } else if (dynamic_cast<MovNode*>(node)) {
        MovNode* movNode = dynamic_cast<MovNode*>(node);
        definitions = {movNode->getSrc()};
    }
    return definitions;
}

std::set<std::pair<std::string, std::string>> AnalysisTools::getAllProgramCopyStatements(BaseNode* entryNode) {
    std::set<std::pair<std::string, std::string>> allCopyStatements;
    //traverse the IR tree, find all MovNodes and add their src and dst to the set
    std::queue<BaseNode*> toExploreQueue;
    toExploreQueue.push(entryNode);
    while (!toExploreQueue.empty()) {
        BaseNode* current = toExploreQueue.front();
        toExploreQueue.pop();

        if (dynamic_cast<MovNode*>(current)) {
            MovNode* movNode = dynamic_cast<MovNode*>(current);
            allCopyStatements.insert(std::make_pair(movNode->getDest(), movNode->getSrc()));
        }

        //add children of the IR node to the queue
        for (BaseNode* child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allCopyStatements;
}

std::set<std::pair<std::string, std::string>> AnalysisTools::getKilledCopyStatementsAtNode(BaseNode* node, std::set<std::pair<std::string, std::string>> &allCopyStatements) {
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

std::set<std::pair<std::string, std::string>> AnalysisTools::getGeneratedCopyStatementsAtNode(BaseNode* node) {
    std::set<std::pair<std::string, std::string>> generatedCopyStatements;
    if (dynamic_cast<MovNode*>(node)) {
        MovNode* movNode = dynamic_cast<MovNode*>(node);
        generatedCopyStatements.insert(std::make_pair(movNode->getDest(), movNode->getSrc()));
    }
    return generatedCopyStatements;
}

std::set<std::string> AnalysisTools::getGeneratedExpressionsAtNode(BaseNode* node) {
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


std::unordered_map<BaseNode*, std::set<std::string>> AnalysisTools::getAllNodesEarliestExpressions(BasicBlock* entryBasicBlock) {
    std::unordered_map<BaseNode*, std::set<std::string>> allNodesEarliestExpressions;

    //get the anticipated[B].in expressions of all nodes
    VBE vbe(entryBasicBlock);
    std::unordered_map<BaseNode*, std::set<std::string>> allNodesAnticipatedExpressions = vbe.getNodeInDataFlowSets();

    //get the available[B].in expressions of all nodes
    AVAIL avail(entryBasicBlock);
    std::unordered_map<BaseNode*, std::set<std::string>> allNodesAvailableExpressions = avail.getNodeInDataFlowSets();

    //For each node: earliest[B] = anticipated[B].in - available[B].in
    for (auto& [node, anticipatedExpressions] : allNodesAnticipatedExpressions) {
        std::set<std::string> earliestExpressions;
        std::set<std::string> availableExpressions = allNodesAvailableExpressions[node];
        std::set_difference(anticipatedExpressions.begin(), anticipatedExpressions.end(), availableExpressions.begin(), availableExpressions.end(), std::inserter(earliestExpressions, earliestExpressions.begin()));
        allNodesEarliestExpressions[node] = earliestExpressions;
    }

    return allNodesEarliestExpressions;
}

std::unordered_map<BaseNode*, std::set<std::string>> AnalysisTools::getAllNodesLatestExpressions(BasicBlock* entryBasicBlock, std::set<std::string> allExpressions, std::vector<BasicBlock*> basicBlocks) {
    std::unordered_map<BaseNode*, std::set<std::string>> allNodesLatestExpressions;

    //get the earliest expressions of all nodes
    std::unordered_map<BaseNode*, std::set<std::string>> allNodesEarliestExpressions = getAllNodesEarliestExpressions(entryBasicBlock);

    //get the in-POST expressions of all nodes
    POST post(entryBasicBlock);
    std::unordered_map<BaseNode*, std::set<std::string>> allNodesInPostExpressions = post.getNodeInDataFlowSets();

    //get node:basicBlock map for all nodes in order to getSuccessorNodes faster
    std::unordered_map<BaseNode*, BasicBlock*> flowgraphNodeToBasicBlockMap = getFlowgraphNodeToBasicBlockMap(basicBlocks);


    //latest[B] = (earliest[B] ∪ in-POST[B]) ∩ (e_useB ∪ ¬(∩ successors S: (earliest[S] ∪ in-POST[S]))
    for (auto& [node, earliestExpressions] : allNodesEarliestExpressions) {     //FOR EACH NODE, find the latest expression set

        //first get (earliest[B] ∪ in-POST[B])
        std::set<std::string> inPostExpressions = allNodesInPostExpressions[node];
        std::set<std::string> earliestOrInPostExpressions;
        std::set_union(earliestExpressions.begin(), earliestExpressions.end(), inPostExpressions.begin(), inPostExpressions.end(), std::inserter(earliestOrInPostExpressions, earliestOrInPostExpressions.begin()));

        //get the used/referenced expressions at the node
        std::set<std::string> usedExpressions = node->getReferencedExpressions();

        //get the successors of the node
        BasicBlock* currentBasicBlock = flowgraphNodeToBasicBlockMap[node];
        std::vector<BaseNode*> successorNodes = getSuccessorNodes(node, currentBasicBlock);

        //intersect over all successors
        std::set<std::string> intersectionOfSuccessorEarliestOrInPostExpressions = allExpressions;
        //for every successor node, find (earliest[S] ∪ in-POST[S])
        for (BaseNode* successor : successorNodes) {
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


std::unordered_map<BaseNode*, BasicBlock*> AnalysisTools::getFlowgraphNodeToBasicBlockMap(std::vector<BasicBlock*> basicBlocks) {
    std::unordered_map<BaseNode*, BasicBlock*> flowgraphNodeToBasicBlockMap;

    for (BasicBlock* basicBlock : basicBlocks) {
        for (BaseNode* node : basicBlock->get_instructions_copy()) {
            flowgraphNodeToBasicBlockMap[node] = basicBlock;
        }
    }
    return flowgraphNodeToBasicBlockMap;
}


std::vector<BaseNode*> AnalysisTools::getSuccessorNodes(BaseNode* node, BasicBlock* currentBasicBlock) {
    std::vector<BaseNode*> successorNodes;

    std::list<BaseNode*> instructions = currentBasicBlock->get_instructions_copy();

    //if its not the last node of the basic block, then the instruction after the current node in the instructions list is the successor
    //otherwise, get the successor basic blocks of the current basic block, and take the first instruction of each successor basic block
    if (node != instructions.back()) {
        std::list<BaseNode*>::const_iterator currentNodeIterator = std::find(instructions.begin(), instructions.end(), node);
        if (currentNodeIterator != instructions.end()) {
            auto nextIterator = std::next(currentNodeIterator);
            if (nextIterator != instructions.end()) {
                successorNodes.push_back(*nextIterator);
            }
        }
    } else {
        //get the successor basic blocks of the current basic block
        std::vector<BasicBlock*> successorBasicBlocks = currentBasicBlock->get_successors();
        //get the first instruction of each successor basic block
        for (BasicBlock* successorBasicBlock : successorBasicBlocks) {
            successorNodes.push_back(successorBasicBlock->get_instructions_copy().front());
        }
    }
    return successorNodes;
}