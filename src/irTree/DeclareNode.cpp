#include "DeclareNode.h"

DeclareNode::DeclareNode(std::string var){
    this->textVector = {"DECLARE", var};
}

BaseNode* DeclareNode::copyNodeOnly() const {
    return new DeclareNode(textVector[1]);
}

std::string DeclareNode::getVar() const {
    return textVector[1];
}

std::string DeclareNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitDeclareNode(this);
}

std::set<std::string> DeclareNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> DeclareNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> DeclareNode::getReferencedExpressions() const {
    return {};
}