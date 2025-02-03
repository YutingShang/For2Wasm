#include "SimpleNode.h"

SimpleNode::SimpleNode() : BaseNode() {}

SimpleNode::SimpleNode(std::string text) : BaseNode(text) {}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::shared_ptr<BaseNode> SimpleNode::getSingleChild() const {
    if (this->children.size() == 1) {
        return this->children[0];
    }
    throw std::runtime_error("SimpleNode has no children for getSingleChild");
}

/////////////////////////TREE MANIPULATION/////////////////////////

void SimpleNode::addChild(std::shared_ptr<BaseNode> child) {
    child->setParent(shared_from_this());

    if (this->children.size() == 0) {
        this->children.push_back(child);
    } else{
        throw std::runtime_error("SimpleNode can only have at most one child");
    }
}

void SimpleNode::addChildAtIndex(std::shared_ptr<BaseNode> child, int index) {
    if (index == 0) {
        addChild(child);
    } else {
        throw std::runtime_error("SimpleNode can only have at most one child");
    }
}

std::shared_ptr<BaseNode> SimpleNode::removeCurrentNodeFromIRTree() {
    int indexInParent = this->getPositionInParent();

    //remove the node from the parent node
    std::shared_ptr<BaseNode> parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    if (parent == nullptr) {
        throw std::runtime_error("Parent node is null. You cannot remove the ENTRY node");
    }
    parent->removeChild(*this);          //remove child to make space for the new nodes

    //add the child to the parent node, if it has one
    std::shared_ptr<BaseNode> child = nullptr;
    if (this->children.size() == 1) {
        child = this->getSingleChild();
        this->removeChild(*child);   //detach the child from the current node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    }

    //no more references to the current node, will be handled by smart pointers

    //return the child node that has replaced the current node
    return child;
}

void SimpleNode::replaceCurrentNodeInIRTree(std::shared_ptr<SimpleNode> newNode) {
    int indexInParent = this->getPositionInParent();

    //attach the new node to the parent node
    std::shared_ptr<BaseNode> parent = this->getParent();      //save a copy of the parent node, removeChild will set the parent to nullptr
    parent->removeChild(*this);
    parent->addChildAtIndex(newNode, indexInParent);

    //attach the new node to the child node
    if (this->children.size() == 1) {
        std::shared_ptr<BaseNode> child = this->getSingleChild();
        this->removeChild(*child);    //did this last night at 4am....
        newNode->addChild(child);    
    }

    //no more references to the current node, will be handled by smart pointers
}

void SimpleNode::insertSandwichChild(std::shared_ptr<SimpleNode> newNode) {
    //remove current child node, and attach it to the newNode
    if (this->children.size() == 1) {
        std::shared_ptr<BaseNode> child = this->getSingleChild();
        this->removeChild(*child);
        newNode->addChild(child);
    } //otherwise just add the newNode as a child

    //attach the newNode to the current node
    this->addChild(newNode);
}