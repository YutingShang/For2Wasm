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

void TestNode::setVar(std::string var) {
    this->textVector[1] = var;
}

std::set<std::string> TestNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getVar())) {
        referencedVariables.insert(getVar());
    }
    return referencedVariables;
}

std::set<std::string> TestNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> TestNode::getGeneratedExpressions() const {
    return {};
}

void TestNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getVar() == oldVar) {
        setVar(newVar);
    }
}