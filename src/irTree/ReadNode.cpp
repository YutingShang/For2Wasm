#include "ReadNode.h"

ReadNode::ReadNode(std::string var) {
    if (!isVariable(var)) {
        throw std::runtime_error("ReadNode variable must be a valid variable");
    }
    this->textVector = {"CALL", "READ", var};
}

BaseNode* ReadNode::cloneContent() const {
    return new ReadNode(getVar());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////
std::string ReadNode::getVar() const {
    return textVector[2];
}

void ReadNode::setVar(std::string var) {
    textVector[2] = var;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string ReadNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitReadNode(this);
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> ReadNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> ReadNode::getDefinedVariables() const {
    return {getVar()};
}

std::set<std::string> ReadNode::getReferencedExpressions() const {
    return {};
}