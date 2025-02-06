#include "InsertableBasicBlock.h"
#include "PlaceholderNode.h"
// #include "BasicBlock.h"

InsertableBasicBlock::InsertableBasicBlock(std::unique_ptr<NodeInsertionStrategy> strategy) : insertionStrategy(std::move(strategy)) {
    //add placeholder instruction node to the instructions list
    addPlaceholderInstructionNode();
}

void InsertableBasicBlock::addPlaceholderInstructionNode() {
    //add placeholder instruction node to the instructions list
    placeholderNode = std::make_shared<PlaceholderNode>();     //the insertable basic block owns the placeholder node
    instructions.push_back(placeholderNode);
}

void InsertableBasicBlock::executeNodeIRInsertion(std::shared_ptr<SimpleNode> nodeToInsert) {
    //use the (private) strategy to insert the node into the IR tree
    insertionStrategy->insertNodeIntoIRTree(nodeToInsert);
    
}

