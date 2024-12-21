#include "EntryNode.h"

EntryNode::EntryNode() : SimpleNode("ENTRY") {}

std::string EntryNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitEntryNode(this);
}

std::set<std::string> EntryNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getDefinedVariables() const {
    return {};
}