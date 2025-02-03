#include "PlaceholderNode.h"

PlaceholderNode::PlaceholderNode() {
    this->textVector = {"PLACEHOLDER"};
}

std::shared_ptr<BaseNode> PlaceholderNode::cloneContent() const {
    return std::make_shared<PlaceholderNode>();
}

/////////////////////////VISITOR PATTERN/////////////////////////
std::string PlaceholderNode::accept(IrBaseVisitor& visitor) {
    //throw error - placeholder node should not be in IR tree
    throw std::runtime_error("Placeholder node should not be in IR tree");
}

/////////////////////////ANALYSIS METHODS/////////////////////////

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

/////////////////////////TREE MANIPULATION/////////////////////////
void PlaceholderNode::addChild(std::shared_ptr<BaseNode> child) {
    //do nothing
}

void PlaceholderNode::addChildAtIndex(std::shared_ptr<BaseNode> child, int index) {
    //do nothing
}

std::shared_ptr<BaseNode> PlaceholderNode::removeCurrentNodeFromIRTree() {
    //throw error - placeholder node should not be in IR tree
    throw std::runtime_error("Placeholder node should not be in IR tree");
}

