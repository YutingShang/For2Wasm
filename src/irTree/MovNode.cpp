#include "MovNode.h"

MovNode::MovNode(std::string dest, std::string src) {
    if (isStringConstant(src)) {
        throw std::runtime_error("Invalid operand : cannot be a string constant");
    }
    textVector = {"MOV", dest, src};
}

std::shared_ptr<BaseNode> MovNode::cloneContent() const {
    return std::make_shared<MovNode>(getDest(), getSrc());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string MovNode::getDest() const {
    return textVector[1];
}

std::string MovNode::getSrc() const {
    return textVector[2];
}

void MovNode::setDest(std::string dest) {
    textVector[1] = dest;
}

void MovNode::setSrc(std::string src) {
    if (isStringConstant(src)) {
        throw std::runtime_error("Invalid operand : cannot be a string constant");
    }
    textVector[2] = src;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string MovNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitMovNode(std::static_pointer_cast<MovNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

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

std::set<std::string> MovNode::getReferencedExpressions() const {
    return {};
}

void MovNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc() == oldVar) {
        setSrc(newVar);
    }
}