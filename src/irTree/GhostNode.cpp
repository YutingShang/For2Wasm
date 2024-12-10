#include "GhostNode.h"

GhostNode::GhostNode(std::string text) : SimpleNode(text) {}

std::string GhostNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitGhostNode(this);
}