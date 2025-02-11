#include "TestNode.h"

TestNode::TestNode(std::string var) {
    if (IRSemantics::isStringConstant(var)) {
        throw std::runtime_error("Invalid test node variable, cannot be string constant: " + var);
    }
    this->var = var;
}

std::string TestNode::getText() const {
    return "TEST " + var;
}

std::shared_ptr<BaseNode> TestNode::cloneContent() const {
    return std::make_shared<TestNode>(getVar());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string TestNode::getVar() const {
    return this->var;
}

void TestNode::setVar(std::string var) {
    if (IRSemantics::isStringConstant(var)) {
        throw std::runtime_error("Invalid test node variable, cannot be string constant: " + var);
    }
    this->var = var;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string TestNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitTestNode(std::static_pointer_cast<TestNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> TestNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getVar())) {
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