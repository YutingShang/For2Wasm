#include "LogicNotNode.h"

LogicNotNode::LogicNotNode(std::string dest, std::string expr) {
    this->textVector = {"NOT", dest, expr};
}

BaseNode* LogicNotNode::cloneContent() const {
    return new LogicNotNode(textVector[1], textVector[2]);
}

std::string LogicNotNode::getDest() const {
    return textVector[1];
}

void LogicNotNode::setDest(std::string dest) {
    textVector[1] = dest;
}

std::string LogicNotNode::getSrc() const {
    return textVector[2];
}

void LogicNotNode::setSrc(std::string src) {
    textVector[2] = src;
}

std::string LogicNotNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLogicNotNode(this);
}

std::set<std::string> LogicNotNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getSrc())) {
        referencedVariables.insert(getSrc());
    }
    return referencedVariables;
}

std::set<std::string> LogicNotNode::getDefinedVariables() const {
    return {getDest()};
}

std::set<std::string> LogicNotNode::getReferencedExpressions() const {
    //e.g. if x = !x, then we say !x has been used/referenced by this node (NOTE: this is different to generated expressions)
    return {"!" + getSrc()};
}

void LogicNotNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc() == oldVar) {
        setSrc(newVar);
    }
}