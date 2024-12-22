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

        static std::vector<BasicBlock*> getBasicBlocks(BasicBlock* entryBasicBlock);

        //computes all the live sets for instructions in the flowgraph
        //takes the start/entry basic block, which contains the entire flowgraph
        static std::vector<std::set<std::string>> computeLiveSets(std::vector<BasicBlock*> &basicBlocks);

        //finds and removes dead code in the flowgraph after the LVA converges
        //returns true if the basic block has been modified, false otherwise
        static bool basicBlockRemoveDeadCode(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &liveSets);

        //computes the live set for instructions within a basic block
        //returns the (in)live set of the basic block (i.e. the inlive of the first instruction)
        static std::set<std::string> basicBlockComputeLiveSet(BasicBlock *basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &liveSets);

};