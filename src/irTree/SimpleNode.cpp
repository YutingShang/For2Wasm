#include "SimpleNode.h"

SimpleNode::SimpleNode(std::string text) : BaseNode(text) {}

void SimpleNode::addChild(BaseNode* child) {
    if (this->children.size() == 0) {
        this->children.push_back(child);
    } else{
        throw std::runtime_error("SimpleNode can only have at most one child");
    }
}

