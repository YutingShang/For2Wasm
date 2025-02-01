#include "TestNode.h"

TestNode::TestNode(std::string var) {
    if (isStringConstant(var)) {
        throw std::runtime_error("Invalid test node variable, cannot be string constant: " + var);
    }
    this->textVector = {"TEST", var};
}

BaseNode* TestNode::cloneContent() const {
    return new TestNode(getVar());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string TestNode::getVar() const {
    return this->textVector[1];
}

void TestNode::setVar(std::string var) {
    if (isStringConstant(var)) {
        throw std::runtime_error("Invalid test node variable, cannot be string constant: " + var);
    }
    this->textVector[1] = var;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string TestNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitTestNode(this);
}

/////////////////////////ANALYSIS METHODS/////////////////////////

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

std::set<std::string> TestNode::getReferencedExpressions() const {
    return {};
}

void TestNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getVar() == oldVar) {
        setVar(newVar);
    }
}