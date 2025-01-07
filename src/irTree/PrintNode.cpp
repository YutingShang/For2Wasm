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

std::set<std::string> PrintNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getSrc())) {
        referencedVariables.insert(getSrc());
    }
    return referencedVariables;
}

std::set<std::string> PrintNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> PrintNode::getGeneratedExpressions() const {
    return {};
}