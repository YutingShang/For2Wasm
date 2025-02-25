#pragma once

#include "BaseNode.h"
#include <map>

//doesn't need the flowgraph, just analyses the IR tree to find perfectly nested loops (LoopCondNode)and tiles them

class LoopTiling {

    public:

        LoopTiling(std::shared_ptr<EntryNode> entryNode);


        //might just return the outerCondNode
        std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> getPerfectlyNestedLoops();


    private:

        std::shared_ptr<EntryNode> entryNode;

        //map of an OUTER loopCondNode to a vector of its perfectly nested INNER loopCondNodes (if there are multiple, they are nested; if there are none, its just an outer loop - can still do loop stripmining)
        std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> loopCondNodesToTile;




        

};