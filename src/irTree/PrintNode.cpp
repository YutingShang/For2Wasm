#include "PrintNode.h"

PrintNode::PrintNode(std::string operand) {
    //operand can be a variable, integer, or string
    this->textVector = {"CALL", "PRINT", operand};
}

BaseNode* PrintNode::cloneContent() const {
    return new PrintNode(getSrc());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string PrintNode::getSrc() const {
    return textVector[2];
}

void PrintNode::setSrc(std::string src) {
    textVector[2] = src;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string PrintNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitPrintNode(this);
}

/////////////////////////ANALYSIS METHODS/////////////////////////

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