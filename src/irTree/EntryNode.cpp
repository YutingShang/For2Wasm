#include "EntryNode.h"

EntryNode::EntryNode() : SimpleNode("ENTRY") {}

BaseNode* EntryNode::cloneContent() const {
    return new EntryNode();
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string EntryNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitEntryNode(this);
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> EntryNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getReferencedExpressions() const {
    return {};
}