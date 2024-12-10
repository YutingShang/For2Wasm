#include "EndBlockNode.h"

EndBlockNode::EndBlockNode(std::string text) 
    : SimpleNode(text) {
} 

std::string EndBlockNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitEndBlockNode(this);
}