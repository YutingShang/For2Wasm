#include "LoopCondNode.h"

LoopCondNode::LoopCondNode(std::string cond, std::string body, std::string step, std::string endloop) {
    this->textVector = {"LOOP", cond, body, step, endloop};
}

void LoopCondNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 4) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopCondNode can only have four children");
    }
}

void LoopCondNode::insertChild(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 4 && index < 4) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("LoopCondNode can only have four children");
    }
}

std::string LoopCondNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();
    tree += "\n" + this->textVector[1] +": " + this->children[0]->stringifyIRTree();      //adds cond label
    tree += "\n" + this->textVector[2] +": " + this->children[1]->stringifyIRTree();      //adds body label
    tree += "\n" + this->textVector[3] +": " + this->children[2]->stringifyIRTree();      //adds step label
    tree += "\n" + this->textVector[4] +": " + this->children[3]->stringifyIRTree();      //adds endloop label
    return tree;
}

std::string LoopCondNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLoopCondNode(this);
}

std::string LoopCondNode::getBodyLabel() const {
    return this->textVector[2];
}

std::string LoopCondNode::getEndLoopLabel() const {
    return this->textVector[4];
}

std::set<std::string> LoopCondNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> LoopCondNode::getDefinedVariables() const {
    return {};
}

