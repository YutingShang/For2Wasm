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

void SimpleNode::insertChild(BaseNode* child, int index) {
    if (index == 0) {
        addChild(child);
    } else {
        throw std::runtime_error("SimpleNode can only have at most one child");
    }
}