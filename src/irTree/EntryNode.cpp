#include "EntryNode.h"

EntryNode::EntryNode() : SimpleNode("ENTRY") {}

std::string EntryNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitEntryNode(this);
}