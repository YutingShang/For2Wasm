#include "IfNode.h"

IfNode::IfNode(std::string condition, std::string thenLabel, std::string endLabel) {

    this->textVector = {"IF", condition, thenLabel, endLabel};
}

void IfNode::addChild(BaseNode* child) {
    child->setParent(this);
    if (this->children.size() < 3) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfNode can only have 3 children");
    }
}

void IfNode::insertChild(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 3 && index < 3) {
        this->children.insert(this->children.begin() + index, child);
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

std::set<std::string> IfNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> IfNode::getDefinedVariables() const {
    return {};
}