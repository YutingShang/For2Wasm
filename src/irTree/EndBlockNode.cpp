#include "EndBlockNode.h"

EndBlockNode::EndBlockNode(std::string text) 
    : SimpleNode(text) {
} 

BaseNode* EndBlockNode::cloneContent() const {
    return new EndBlockNode(textVector[0]);
}

std::string EndBlockNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitEndBlockNode(this);
}

std::set<std::string> EndBlockNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> EndBlockNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> EndBlockNode::getReferencedExpressions() const {
    return {};
}