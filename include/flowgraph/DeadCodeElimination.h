#pragma once

#include "BasicBlock.h"
#include <set>
#include <queue>
#include <map>

//static class for dead code elimination, doesn't need to keep track of state

class DeadCodeElimination {

    public:

        DeadCodeElimination() = delete;

        static void iterateDeadCodeElimination(BasicBlock* entryBasicBlock);

        //runs LVA then checks for dead code - ONCE
        //returns true if code was deleted, false otherwise
        static bool deadCodeEliminationOnce(BasicBlock* entryBasicBlock);


    private:

        //finds and removes dead code in the flowgraph after the LVA converges
        //returns true if the basic block has been modified, false otherwise
        static bool basicBlockRemoveDeadCode(BasicBlock* basicBlock, std::vector<BasicBlock*> &basicBlocks, std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> &nodeLiveSets);

};