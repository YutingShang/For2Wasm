#include "EntryNode.h"

EntryNode::EntryNode() : SimpleNode("ENTRY") {}

BaseNode* EntryNode::cloneContent() const {
    return new EntryNode();
}

std::string EntryNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitEntryNode(this);
}

std::set<std::string> EntryNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getReferencedExpressions() const {
    return {};
}