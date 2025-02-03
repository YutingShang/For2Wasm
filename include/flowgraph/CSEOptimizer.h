#pragma once

#include "BasicBlock.h"
#include "AnalysisTools.h"
#include "AVAIL.h"
#include <map>

//not a static class, unlike DeadCodeElimination
//needs to keep the temporary variable count as state for a particular program
class CSEOptimizer {

    public:

        //constructor - will set the member variables
        // should initialise the nextProgramTempVariableCount to 0 if its the first time CSE is run
        // the next time, feed the previous nextProgramTempVariableCount into the constructor
        CSEOptimizer(BasicBlock* entryBasicBlock, int nextProgramTempVariableCount);

        //runs CSE once, returns true if code was deleted, false otherwise
        bool commonSubexpressionEliminationOnce();

        //runs CSE iteratively until no more common subexpressions are found
        ///TODO: maybe remove this, since its the same as commonSubexpressionEliminationOnce(). Assert that the result is false
        bool iterateCommonSubexpressionElimination();

        //runs CSE + Copy Propagation repeatedly until no more common subexpressions are found
        bool iterateCSE_CopyPropagation();

        //returns the next program temp variable count
        int getNextProgramTempVariableCount();

    private:
        
        //member variables
        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;            //vector of all basic blocks in the program
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> nodeAvailSets;      //vector of available expressions for each instruction node

        //the next temporary variable count, initialised to 0
        //these are _s program temporary variables which are added to the resulting program(not to be confused with _t temp variables for internal use)
        int nextProgramTempVariableCount;


        //finds and removes common subexpressions in the flowgraph (and the IR tree) after the CSE converges
        //returns true if the basic block has been modified, false otherwise
        bool basicBlockRemoveCommonSubexpressions(BasicBlock* basicBlock);

        //helper function in CSE when going backwards on each control path
        //in each basic block, find the first occurrence of the expression
        //and replace it with a the temporary variable
        //beginBackwardsFromThisNode is the node to start searching backwards from (since for the current basic block, we start searching from the current instruction, not from the end of the basic block)
            //if it is nullptr, then default to starting from the end of the basic block
        bool basicBlockBackwardsFindAndReplaceExpression(BasicBlock* basicBlock, std::string &expressionToFind, std::string &tempVar, std::shared_ptr<BaseNode> beginBackwardsFromThisNode = nullptr);

        //returns a new temporary variable name _s
        //also adds a declaration statement to the start of the program 
        std::string addNewProgramTempVariable();
  
    
};

