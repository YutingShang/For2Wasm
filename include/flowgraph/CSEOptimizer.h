#pragma once

#include "BasicBlock.h"
#include "SimpleNode.h"
#include "ExpressionNode.h"
#include "MovNode.h"
#include "DeclareNode.h"
#include <set>
#include <queue>
#include <stack>
#include <regex>

//not a static class, unlike DeadCodeElimination
//needs to keep the temporary variable count as state for a particular program
class CSEOptimizer {

    public:

        //
        CSEOptimizer();

        //runs CSE once, returns true if code was deleted, false otherwise
        bool commonSubexpressionEliminationOnce(BasicBlock* entryBasicBlock, std::set<std::string> &allExpressions);

        //runs CSE iteratively until no more common subexpressions are found
        void iterateCommonSubexpressionElimination(BasicBlock* entryBasicBlock);

    private:

        //returns a vector of all the basic blocks in the flowgraph
        //uses BFS to get the basic blocks roughly in flowgraph order
        //entryBasicBlock should be the first basic block in the returned vector
        std::vector<BasicBlock*> getBasicBlocks(BasicBlock* entryBasicBlock);

        //returns the universe of all expressions in the program
        std::set<std::string> getAllProgramExpressions(BaseNode* root);

        //returns the set of expressions Ex that are killed by the node (specific to a program)
        std::set<std::string> getKilledProgramExpressions(BaseNode* node, std::set<std::string> &allExpressions);

        //computes the available expressions for each basic block
        //takes in a vector of basic blocks, returns a vector of available expressions for each basic block
        std::vector<std::set<std::string>> computeAvailSets(std::vector<BasicBlock*> &basicBlocks, std::set<std::string> &allExpressions);

        //computes the available expressions within a single basic block
        std::set<std::string> basicBlockComputeAvailSet(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets, std::set<std::string> &allExpressions);

        //finds and removes common subexpressions in the flowgraph (and the IR tree) after the CSE converges
        //returns true if the basic block has been modified, false otherwise
        bool basicBlockRemoveCommonSubexpressions(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets, std::set<std::string> &allExpressions);

        //helper function in CSE when going backwards on each control path
        //in each basic block, find the first occurrence of the expression
        //and replace it with a the temporary variable
        //beginBackwardsFromThisNode is the node to start searching backwards from (since for the current basic block, we start searching from the current instruction, not from the end of the basic block)
            //if it is nullptr, then default to starting from the end of the basic block
        bool basicBlockBackwardsFindAndReplaceExpression(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::string &expressionToFind, std::string &tempVar, BaseNode* beginBackwardsFromThisNode = nullptr);

        //returns a new temporary variable name _s
        //also adds a declaration statement to the start of the program 
        std::string getNewProgramTempVariable(BasicBlock* entryBasicBlock);

        //the next temporary variable count, initialised to 0
        //these are _s program temporary variables which are added to the resulting program(not to be confused with _t temp variables for internal use)
        int nextProgramTempVariableCount = 0;

    
};

