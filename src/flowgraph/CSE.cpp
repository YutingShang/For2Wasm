#include "CSE.h"
#include <iostream>


void CSE::iterateCommonSubexpressionElimination(BasicBlock *entryBasicBlock)
{
    // bool runCSE = true;
    // while (runCSE)
    // {
    //     runCSE = commonSubexpressionEliminationOnce(entryBasicBlock);
    // }
    BaseNode* root = entryBasicBlock->get_instructions().front();
    std::set<std::string> allExpressions = getAllProgramExpressions(root);
    std::cout << "All expressions: {" << std::endl;
    for (const std::string& expr : allExpressions) {
        std::cout << expr << std::endl;
    }
    std::cout << "}" << std::endl;


    //visit each node, and get the killed expressions
    BaseNode* node = root;
    std::queue<BaseNode*> toExploreQueue;
    toExploreQueue.push(node);
    while (!toExploreQueue.empty()) {
        BaseNode* current = toExploreQueue.front();
        toExploreQueue.pop();
        std::cout << "Current node: " << current->getText() << std::endl;
        std::set<std::string> killedExpressions = getKilledProgramExpressions(current, allExpressions);
        std::cout << "Killed expressions: {" << std::endl;
        for (const std::string& expr : killedExpressions) {
            std::cout << expr << std::endl;
        }
        std::cout << "}" << std::endl;
   
        //add children to the queue
        for (BaseNode* child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
}


bool CSE::commonSubexpressionEliminationOnce(BasicBlock *entryBasicBlock)
{
    // Analysis: AVAIL
    std::vector<BasicBlock *> basicBlocks = getBasicBlocks(entryBasicBlock);
    std::vector<std::set<std::string>> availSets = computeAvailSets(basicBlocks);

    // Transformation: Remove common subexpressions
    bool removed = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        removed |= basicBlockRemoveCommonSubexpressions(basicBlock, basicBlocks, availSets);
    }

    return removed;
}

///NOTE: slightly modified from DeadCodeElimination.cpp - does not reverse the basic blocks
std::vector<BasicBlock *> CSE::getBasicBlocks(BasicBlock *entryBasicBlock)
{
    // first get a list of all the basic blocks in the flowgraph
    // we want to propagate information down the flowgraph
    // since CSE is a forwards analysis
    // so we visit the basic blocks in forward order

    std::vector<BasicBlock *> basicBlocks;

    // get all the basic blocks in the flowgraph by BFS, and output into the basicBlocks vector
    std::queue<BasicBlock *> toExploreQueue;
    toExploreQueue.push(entryBasicBlock);
    std::set<BasicBlock *> seen;
    seen.insert(entryBasicBlock);

    while (!toExploreQueue.empty())
    {
        BasicBlock *current = toExploreQueue.front();
        basicBlocks.push_back(current); // visit the current basic block
        toExploreQueue.pop();

        // add all the successors to the queue if they are not already seen
        for (BasicBlock *successor : current->get_successors())
        {
            if (seen.find(successor) == seen.end())
            { // check 'seen' set
                toExploreQueue.push(successor);
                seen.insert(successor);
            }
        }
    }

    return basicBlocks;

}

std::set<std::string> CSE::getAllProgramExpressions(BaseNode* root) {
    std::set<std::string> allExpressions;

    //IR tree traversal (not a graph)
    std::queue<BaseNode*> toExploreQueue;
    toExploreQueue.push(root);
    while (!toExploreQueue.empty()) {
        BaseNode* current = toExploreQueue.front();
        toExploreQueue.pop();
        //get generated expressions, insert into allExpressions set
        const std::set<std::string>& generatedExpressions = current->getGeneratedExpressions();
        allExpressions.insert(generatedExpressions.begin(), generatedExpressions.end());
        //add children to the queue
        for (BaseNode* child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allExpressions;
}

std::set<std::string> CSE::getKilledProgramExpressions(BaseNode* node, std::set<std::string> &allExpressions) {
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

std::vector<std::set<std::string>> CSE::computeAvailSets(std::vector<BasicBlock*> &basicBlocks)
{
    /// NOTE: this is the AVAIL algorithm


}

std::set<std::string> CSE::basicBlockComputeAvailSet(BasicBlock *basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets)
{

}

bool CSE::basicBlockRemoveCommonSubexpressions(BasicBlock *basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets)
{

}

