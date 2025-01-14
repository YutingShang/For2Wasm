#pragma once

#include "BasicBlock.h"
#include "AnalysisTools.h"

// this is the analysis for "Available Copy Statements"
// used for copy and constant propagation
// similar to the AVAIL analysis, but for copy statements instead of expressions

class ACS {

    public:

        ACS(BasicBlock* entryBasicBlock);

        //get the available copy statements for each basic block
        std::vector<std::set<std::pair<std::string, std::string>>> getAvailCopies();

        std::vector<BasicBlock*> getBasicBlocksUsed();

        //get the set of all copy statements (from MovNodes) in the program
        std::set<std::pair<std::string, std::string>> getAllCopyStatements();


    private:

        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;
        //set of all copy statements (from MovNodes) in the program
        std::set<std::pair<std::string, std::string>> allCopyStatements;
        //vector for available copy statements for each basic block
        //for each basic block, there is a set of tuples of the form (var, replacementVar), for the available copy statements
        std::vector<std::set<std::pair<std::string, std::string>>> availCopies;   

        //computes the available copy statements for each basic block
        void computeAvailableCopySets();

        //computes the available copy statements for a single basic block
        std::set<std::pair<std::string, std::string>> basicBlockComputeAvailableCopySets(BasicBlock* basicBlock);

        void printAvailCopies();

};