#pragma once

#include "AnalysisTools.h"

//instance class for live variable analysis for a flowgraph
class LVA {

    public:

        //constructor - will compute the basic blocks in the flowgraph, and set it in the basicBlocks vector
        LVA(BasicBlock* entryBasicBlock);

        //returns the live sets for entire flowgraph (per basic block)
        std::vector<std::set<std::string>> getLiveSets();

        //returns the basic blocks in the flowgraph used for the LVA analysis
        std::vector<BasicBlock*> getBasicBlocks();

    private:

        //member variables
        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;            //vector of all basic blocks in the program
        std::vector<std::set<std::string>> liveSets;      //vector of live sets for each basic block

        //computes all the live sets for instructions in the flowgraph
        //takes in a vector of basic blocks, computes a vector of live sets for each basic block
        //sets the liveSets vector
        void computeLiveSets();

        //computes the live set for instructions within a basic block
        //returns the (in)live set of the basic block (i.e. the inlive of the first instruction)
        std::set<std::string> basicBlockComputeLiveSet(BasicBlock *basicBlock);

};