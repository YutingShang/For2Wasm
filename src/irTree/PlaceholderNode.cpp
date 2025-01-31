#include "PlaceholderNode.h"

PlaceholderNode::PlaceholderNode() {
    this->textVector = {"PLACEHOLDER"};
}

BaseNode* PlaceholderNode::copyNodeOnly() const {
    return new PlaceholderNode();
}

void PlaceholderNode::addChild(BaseNode* child) {
    //do nothing
}

void PlaceholderNode::addChildAtIndex(BaseNode* child, int index) {
    //do nothing
}

std::string PlaceholderNode::accept(IrBaseVisitor* visitor) {
    //throw error - placeholder node should not be in IR tree
    throw std::runtime_error("Placeholder node should not be in IR tree");
}

BaseNode* PlaceholderNode::removeCurrentNodeFromIRTree() {
    //throw error - placeholder node should not be in IR tree
    throw std::runtime_error("Placeholder node should not be in IR tree");
}


std::set<std::string> PlaceholderNode::getReferencedVariables() const {
    //no variables
    return {};
}

std::set<std::string> PlaceholderNode::getDefinedVariables() const {
    //no variables
    return {};
}

std::set<std::string> PlaceholderNode::getReferencedExpressions() const {
    //no expressions
    return {};
}
