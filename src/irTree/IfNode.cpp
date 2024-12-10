#include "IfNode.h"

IfNode::IfNode(std::string text) : BaseNode(text) {

    if (this->textVector.size() != 4 && this->textVector.size() != 5) {
        throw std::runtime_error("IfNode must have 4 or 5 tokens");
    }
}

void IfNode::addChild(BaseNode* child) {
    if (this->textVector.size() == 4 && this->children.size() < 3) {
        this->children.push_back(child);
    } else if (this->textVector.size() == 5 && this->children.size() < 4) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfNode can only have 3 or 4 children");
    }
}

std::string IfNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();

    for (int i=1; i<this->textVector.size(); i++){
        tree += "\n" + this->textVector[i] +": " + this->children[i-1]->stringifyIRTree();
    }

    return tree;
}

std::string IfNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitIfNode(this);
}