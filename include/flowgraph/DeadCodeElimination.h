#pragma once

#include "BasicBlock.h"
#include <set>
#include <queue>

class DeadCodeElimination {

    public:

        DeadCodeElimination() = delete;

        static void iterateDeadCodeElimination(BasicBlock* entryBasicBlock);

        //runs LVA then checks for dead code - ONCE
        //returns true if code was deleted, false otherwise
        static bool deadCodeEliminationOnce(BasicBlock* entryBasicBlock);


    private:

        //returns a vector of all the basic blocks in the flowgraph
        //uses BFS to get the basic blocks roughly in flowgraph order, then reverses it for backwards analysis
        static std::vector<BasicBlock*> getBasicBlocksReverse(BasicBlock* entryBasicBlock);

        //computes all the live sets for instructions in the flowgraph
        //takes in a vector of basic blocks, returns a vector of live sets for each basic block
        static std::vector<std::set<std::string>> computeLiveSets(std::vector<BasicBlock*> &basicBlocks);

        //finds and removes dead code in the flowgraph after the LVA converges
        //returns true if the basic block has been modified, false otherwise
        static bool basicBlockRemoveDeadCode(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &liveSets);

        //computes the live set for instructions within a basic block
        //returns the (in)live set of the basic block (i.e. the inlive of the first instruction)
        static std::set<std::string> basicBlockComputeLiveSet(BasicBlock *basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &liveSets);

};