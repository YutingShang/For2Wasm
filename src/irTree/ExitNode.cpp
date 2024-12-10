#include "ExitNode.h"

ExitNode::ExitNode() 
    : SimpleNode("EXIT") {
} 

std::string ExitNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitExitNode(this);
}