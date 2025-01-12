#pragma once

#include "AnalysisTools.h"

//instance class for available expression analysis (need to keep track of basic blocks state for a flowgraph)

class AVAIL {

    public:

        //constructor - will compute the basic blocks in the flowgraph, and set it in the basicBlocks vector
        AVAIL(BasicBlock* entryBasicBlock);

        //returns the available expressions for entire flowgraph (per basic block)
        std::vector<std::set<std::string>> getAvailSets();

        //returns the basic blocks in the flowgraph that the AVAIL analysis was performed on
        std::vector<BasicBlock*> getBasicBlocks();

        //returns the universe of all expressions in the program
        std::set<std::string> getAllExpressions();


    private:

        //member variables
        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;            //vector of all basic blocks in the program
        std::set<std::string> allExpressions;            //universe of all expressions in the program
        std::vector<std::set<std::string>> availSets;      //vector of available expressions for each basic block

        //computes the available expressions for each basic block
        //takes in a vector of basic blocks, computes a vector of available expressions for each basic block
        //sets the availSets vector
        void computeAvailSets();

        //computes the available expressions within a single basic block
        std::set<std::string> basicBlockComputeAvailSet(BasicBlock* basicBlock);

};