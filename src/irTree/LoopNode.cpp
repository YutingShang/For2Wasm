#include "LoopNode.h"

LoopNode::LoopNode(std::string text) : BaseNode(text) {
    if (this->textVector.size() != 3) {
        throw std::runtime_error("LoopNode must have 3 tokens");
    }
}

void LoopNode::addChild(BaseNode* child) {
    if (this->children.size() < 2) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopNode can only have two children");
    }
}

std::string LoopNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();
    tree += "\n" + this->textVector[1] +": " + this->children[0]->stringifyIRTree();      //adds body label
    tree += "\n" + this->textVector[2] +": " + this->children[1]->stringifyIRTree();      //adds exit label
    return tree;
}

