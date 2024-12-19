#include "TestNode.h"

TestNode::TestNode(std::string var) {
    this->textVector = {"TEST", var};
}

std::string TestNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitTestNode(this);
}

std::string TestNode::getVar() const {
    return this->textVector[1];
}