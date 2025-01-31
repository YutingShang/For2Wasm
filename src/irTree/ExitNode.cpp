#include "ExitNode.h"

ExitNode::ExitNode() 
    : SimpleNode("EXIT") {
} 

BaseNode* ExitNode::copyNodeOnly() const {
    return new ExitNode();
}

std::string ExitNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitExitNode(this);
}

std::set<std::string> ExitNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> ExitNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> ExitNode::getReferencedExpressions() const {
    return {};
}