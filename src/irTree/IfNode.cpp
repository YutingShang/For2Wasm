#include "IfNode.h"

IfNode::IfNode(std::string condition, std::string thenLabel, std::string endLabel) {

    this->textVector = {"IF", condition, thenLabel, endLabel};
}

void IfNode::addChild(BaseNode* child) {
    if (this->children.size() < 3) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfNode can only have 3 children");
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