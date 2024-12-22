#include "LoopNode.h"

LoopNode::LoopNode(std::string body, std::string exit){
    this->textVector = {"LOOP", body, exit};
}

void LoopNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 2) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopNode can only have two children");
    }
}

void LoopNode::insertChild(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 2 && index < 2) {
        this->children.insert(this->children.begin() + index, child);
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

std::string LoopNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLoopNode(this);
}

std::string LoopNode::getBodyLabel() const {
    return this->textVector[1];
}

std::string LoopNode::getExitLabel() const {
    return this->textVector[2];
}

std::set<std::string> LoopNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> LoopNode::getDefinedVariables() const {
    return {};
}