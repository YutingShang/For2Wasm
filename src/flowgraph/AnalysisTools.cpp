#include "AnalysisTools.h"
#include <set>
#include <vector>
#include <queue>
#include <regex>
#include <iostream>
#include "ExpressionNode.h"
#include "MovNode.h"

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
        const std::set<std::string>& generatedExpressions = current->getGeneratedExpressions();
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

std::unordered_map<std::string, std::set<BaseNode*>> AnalysisTools::getAllProgramDefinitionPoints(BaseNode* entryNode) {
    std::unordered_map<std::string, std::set<BaseNode*>> allDefinitionPoints;

    //IR tree traversal (not a graph)
    std::queue<BaseNode*> toExploreQueue;
    toExploreQueue.push(entryNode);
    while (!toExploreQueue.empty()) {
        //for each IR node
        BaseNode* current = toExploreQueue.front();
        toExploreQueue.pop();

        //get generated expressions
        // const std::set<std::string>& generatedExpressions = current->getGeneratedExpressions();

        //get defined variables
        std::set<std::string> definedVariables = current->getDefinedVariables();
        if (!definedVariables.empty()) {  //if the node defines a variable

            assert(definedVariables.size() == 1);     //assume only 1 variable is defined 
            std::string definedVariable = *definedVariables.begin();

            // for (const auto &generatedExpression : generatedExpressions) {
            allDefinitionPoints[definedVariable].insert(current);    //add expressions to set of definitions for defined_variable
            // }
        }

        //add children of the IR node to the queue
        for (BaseNode* child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allDefinitionPoints;
}

std::set<std::string> AnalysisTools::getDefinitionsAtNode(BaseNode* node) {
    std::set<std::string> definitions;
    if (dynamic_cast<ExpressionNode*>(node)) {
        definitions = node->getGeneratedExpressions();
    } else if (dynamic_cast<MovNode*>(node)) {
        MovNode* movNode = dynamic_cast<MovNode*>(node);
        definitions = {movNode->getSrc()};
    }
    return definitions;
}
