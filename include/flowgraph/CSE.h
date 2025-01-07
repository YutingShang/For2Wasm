#pragma once

#include "BasicBlock.h"
#include <set>
#include <queue>
#include <regex>

class CSE {

    public:

        //remove constructor for static class
        CSE() = delete;

        //runs CSE once, returns true if code was deleted, false otherwise
        static bool commonSubexpressionEliminationOnce(BasicBlock* entryBasicBlock);

        //runs CSE iteratively until no more common subexpressions are found
        static void iterateCommonSubexpressionElimination(BasicBlock* entryBasicBlock);

    private:

        //returns a vector of all the basic blocks in the flowgraph
        //uses BFS to get the basic blocks roughly in flowgraph order
        static std::vector<BasicBlock*> getBasicBlocks(BasicBlock* entryBasicBlock);

        //returns the universe of all expressions in the program
        static std::set<std::string> getAllProgramExpressions(BaseNode* root);

        //returns the set of expressions Ex that are killed by the node (specific to a program)
        static std::set<std::string> getKilledProgramExpressions(BaseNode* node, std::set<std::string> &allExpressions);

        //computes the available expressions for each basic block
        //takes in a vector of basic blocks, returns a vector of available expressions for each basic block
        static std::vector<std::set<std::string>> computeAvailSets(std::vector<BasicBlock*> &basicBlocks);

        //computes the available expressions within a single basic block
        static std::set<std::string> basicBlockComputeAvailSet(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets);

        //finds and removes common subexpressions in the flowgraph (and the IR tree) after the CSE converges
        //returns true if the basic block has been modified, false otherwise
        static bool basicBlockRemoveCommonSubexpressions(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets);
};