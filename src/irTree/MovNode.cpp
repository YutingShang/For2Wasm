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

void MovNode::setSrc(std::string src) {
    textVector[2] = src;
}

std::string MovNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitMovNode(this);
}

std::set<std::string> MovNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getSrc())) {
        referencedVariables.insert(getSrc());
    }
    return referencedVariables;
}

std::set<std::string> MovNode::getDefinedVariables() const {
    return {getDest()};
}

std::set<std::string> MovNode::getGeneratedExpressions() const {
    return {};
}

void MovNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc() == oldVar) {
        setSrc(newVar);
    }
}