#include "InsertableBasicBlock.h"
#include "PlaceholderNode.h"
// #include "BasicBlock.h"

InsertableBasicBlock::InsertableBasicBlock(NodeInsertionStrategy* strategy) : insertionStrategy(strategy) {
    //add placeholder instruction node to the instructions list
    addPlaceholderInstructionNode();
}

void InsertableBasicBlock::addPlaceholderInstructionNode() {
    //add placeholder instruction node to the instructions list
    instructions.push_back(new PlaceholderNode());
}

// std::list<BaseNode*>::iterator InsertableBasicBlock::removePlaceholderInstructionNode(std::list<BaseNode*>::iterator it) {
//     //remove the placeholder instruction node from the instructions list
//     PlaceholderNode* placeholderNode = dynamic_cast<PlaceholderNode*>(*it);
//     assert(placeholderNode != nullptr);
//     it = instructions.erase(it);

//     assert(placeholderNode->getParent() == nullptr);
//     assert(placeholderNode->getChildren().empty());
//     delete placeholderNode;

//     return it;
// }

void InsertableBasicBlock::executeNodeIRInsertion(std::list<BaseNode*>::iterator it, SimpleNode* nodeToInsert) {
    //use the strategy to insert the node into the IR tree
    insertionStrategy->insertNodeIntoIRTree(nodeToInsert);
    
    //insert instruction node into the instructions list
    instructions.insert(it, nodeToInsert);

    //same iterator still valid
}

