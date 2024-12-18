#include "MovNode.h"

MovNode::MovNode(std::string dest, std::string src)
{
    this->textVector = {"MOV", dest, src};
}

std::string MovNode::getDest() const
{
    return this->textVector[1];
}

std::string MovNode::getSrc() const
{
    return this->textVector[2];
}

std::string MovNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitMovNode(this);
}