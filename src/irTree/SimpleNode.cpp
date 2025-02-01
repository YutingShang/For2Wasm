#include "SimpleNode.h"

SimpleNode::SimpleNode() : BaseNode() {}

SimpleNode::SimpleNode(std::string text) : BaseNode(text) {}

void SimpleNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() == 0) {
        this->children.push_back(child);
    } else{
        throw std::runtime_error("SimpleNode can only have at most one child");
    }
}

void SimpleNode::addChildAtIndex(BaseNode* child, int index) {
    if (index == 0) {
        addChild(child);
    } else {
        throw std::runtime_error("SimpleNode can only have at most one child");
    }
}

BaseNode* SimpleNode::removeCurrentNodeFromIRTree() {
    int indexInParent = this->getPositionInParent();

    //remove the node from the parent node
    BaseNode* parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    if (parent == nullptr) {
        throw std::runtime_error("Parent node is null. You cannot remove the ENTRY node");
    }
    this->parent->removeChild(this);          //remove child to make space for the new nodes

    //add the child to the parent node, if it has one
    BaseNode* child = nullptr;
    if (this->children.size() == 1) {
        child = this->children[0];
        this->removeChild(child);   //detach the child from the current node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    }

    // delete the current node from memory
    // delete this;

    //return the child node that has replaced the current node
    return child;
}

void SimpleNode::replaceCurrentNodeInIRTree(SimpleNode* newNode) {
    int indexInParent = this->getPositionInParent();

    //attach the new node to the parent node
    BaseNode* parent = this->getParent();      //save a copy of the parent node, removeChild will set the parent to nullptr
    parent->removeChild(this);
    parent->addChildAtIndex(newNode, indexInParent);

    //attach the new node to the child node
    if (this->children.size() == 1) {
        BaseNode* child = this->children[0];
        this->removeChild(child);    //did this last night at 4am....
        newNode->addChild(child);    
    }

    //delete the current node from memory
    delete this;
}

void SimpleNode::insertSandwichChild(SimpleNode* newNode) {
    //remove current child node, and attach it to the newNode
    if (this->children.size() == 1) {
        BaseNode* child = this->children[0];
        this->removeChild(child);
        newNode->addChild(child);
    } //otherwise just add the newNode as a child

    //attach the newNode to the current node
    this->addChild(newNode);
}