#include "ReadNode.h"

ReadNode::ReadNode(std::string var) {
    this->textVector = {"CALL", "READ", var};
}


std::string ReadNode::getVar() const {
    return textVector[2];
}

std::string ReadNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitReadNode(this);
}