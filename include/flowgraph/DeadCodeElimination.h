#pragma once

#include "BasicBlock.h"
#include <set>
#include <queue>
#include <map>

//class for dead code elimination, keeps track of the entry node, redraws the flowgraph after each iteration

class DeadCodeElimination {

    public:

        DeadCodeElimination(std::shared_ptr<EntryNode> entryNode);

        //runs DCE iteratively until no more dead code is found
        void iterateDeadCodeElimination();

        //runs LVA then checks for dead code - ONCE
        //returns true if code was deleted, false otherwise
        bool deadCodeEliminationOnce();


    private:

        //finds and removes dead code in a flowgraph node 
        //returns true if the node has been removed, false otherwise
        bool removeDeadCodeForNode(std::shared_ptr<BaseNode> instruction);

        std::shared_ptr<EntryNode> entryNode;
        std::shared_ptr<BasicBlock> startBasicBlock = nullptr;
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> nodeLiveSets;     //stores the live sets for each node in the flowgraph
};