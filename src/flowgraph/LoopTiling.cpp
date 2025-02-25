#include "LoopTiling.h"
#include "IrNestedLoopVisitor.h"
#include "EntryNode.h"
#include "LoopCondNode.h"
#include <iostream>

LoopTiling::LoopTiling(std::shared_ptr<EntryNode> entryNode): entryNode(entryNode) {

}

std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> LoopTiling::getPerfectlyNestedLoops() {

    //Note (actually implemented using a visitor)
    //visit all the nodes in the graph, in dfs (left to right order)
    //if we see a loop cond node, and it is the outer loop, add it to the map, with an empty vector
    //if we see a loop cond node, and it is the inner loop, add it to the vector of the outer loop
    //until we exit that outer loop (keep track of how many layers of LOOPS we are in and ENDLOOPS using a stack)

    IrNestedLoopVisitor nestedLoopVisitor;
    entryNode->accept(nestedLoopVisitor);
    loopCondNodesToTile = nestedLoopVisitor.getPerfectlyNestedLoops();

    for (auto &keyValuePair : loopCondNodesToTile) {
        std::cerr << "Outer loop: " << keyValuePair.first->getLabelNumber() << std::endl;
        for (auto &innerLoop : keyValuePair.second) {
            std::cerr << "  Inner loop: " << innerLoop->getLabelNumber() << std::endl;
        }
    }

    return loopCondNodesToTile;
}