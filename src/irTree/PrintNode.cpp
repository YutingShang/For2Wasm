#include "PrintNode.h"

PrintNode::PrintNode(std::string operand) {
    this->textVector = {"CALL", "PRINT", operand};
}

BaseNode* PrintNode::copyNodeOnly() const {
    return new PrintNode(textVector[2]);
}

std::string PrintNode::getSrc() const {
    return textVector[2];
}

void PrintNode::setSrc(std::string src) {
    textVector[2] = src;
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

std::set<std::string> PrintNode::getReferencedExpressions() const {
    return {};
}

void PrintNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc() == oldVar) {
        setSrc(newVar);
    }
}