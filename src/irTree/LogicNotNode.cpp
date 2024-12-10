#include "LogicNotNode.h"

LogicNotNode::LogicNotNode(std::string dest, std::string expr) {
    this->textVector = {"NOT", dest, expr};
}

std::string LogicNotNode::getDest() const {
    return textVector[1];
}

std::string LogicNotNode::getSrc() const {
    return textVector[2];
}

std::string LogicNotNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLogicNotNode(this);
}