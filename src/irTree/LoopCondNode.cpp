#include "LoopCondNode.h"

LoopCondNode::LoopCondNode(std::string init, std::string cond, std::string body, std::string step, std::string endloop) {
    this->textVector = {"LOOP", init, cond, body, step, endloop};
}

void LoopCondNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 5) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopCondNode can only have five children");
    }
}

void LoopCondNode::insertChild(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 5 && index < 5) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("LoopCondNode can only have five children");
    }
}

std::string LoopCondNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();
    tree += "\n" + this->textVector[1] +": " + this->children[0]->stringifyIRTree();      //adds init label
    tree += "\n" + this->textVector[2] +": " + this->children[1]->stringifyIRTree();      //adds cond label
    tree += "\n" + this->textVector[3] +": " + this->children[2]->stringifyIRTree();      //adds body label
    tree += "\n" + this->textVector[4] +": " + this->children[3]->stringifyIRTree();      //adds step label
    tree += "\n" + this->textVector[5] +": " + this->children[4]->stringifyIRTree();      //adds endloop label
    return tree;
}

std::string LoopCondNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLoopCondNode(this);
}

std::string LoopCondNode::getBodyLabel() const {
    return this->textVector[3];
}

std::string LoopCondNode::getEndLoopLabel() const {
    return this->textVector[5];
}

std::set<std::string> LoopCondNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> LoopCondNode::getDefinedVariables() const {
    return {};
}

