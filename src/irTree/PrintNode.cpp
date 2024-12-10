#include "PrintNode.h"

PrintNode::PrintNode(std::string operand) {
    this->textVector = {"CALL", "PRINT", operand};
}

std::string PrintNode::getSrc() const {
    return textVector[2];
}

std::string PrintNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitPrintNode(this);
}