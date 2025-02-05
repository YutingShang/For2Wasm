#pragma once

#include "BasicBlock.h"
#include <set>
#include <queue>
#include <map>

//static class for dead code elimination, doesn't need to keep track of state

class DeadCodeElimination {

    public:

        DeadCodeElimination() = delete;

        static void iterateDeadCodeElimination(std::shared_ptr<BasicBlock> entryBasicBlock);

        //runs LVA then checks for dead code - ONCE
        //returns true if code was deleted, false otherwise
        static bool deadCodeEliminationOnce(std::shared_ptr<BasicBlock> entryBasicBlock);


    private:

        //finds and removes dead code in the flowgraph after the LVA converges
        //returns true if the basic block has been modified, false otherwise
        static bool basicBlockRemoveDeadCode(std::shared_ptr<BasicBlock> basicBlock, std::vector<std::shared_ptr<BasicBlock>> &basicBlocks, std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> &nodeLiveSets);

};