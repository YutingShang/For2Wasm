#include "MovNode.h"

MovNode::MovNode(std::string dest, std::string src) {
    if (IRSemantics::isStringConstant(src)) {
        throw std::runtime_error("Invalid operand : cannot be a string constant");
    }
    this->dest = dest;
    this->src = src;
}

std::string MovNode::getText() const {
    return "MOV " + dest + " " + src;
}

std::shared_ptr<BaseNode> MovNode::cloneContent() const {
    return std::make_shared<MovNode>(getDest(), getSrc());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string MovNode::getDest() const {
    return dest;
}

std::string MovNode::getSrc() const {
    return src;
}

void MovNode::setDest(std::string dest) {
    this->dest = dest;
}

void MovNode::setSrc(std::string src) {
    if (IRSemantics::isStringConstant(src)) {
        throw std::runtime_error("Invalid operand : cannot be a string constant");
    }
    this->src = src;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string MovNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitMovNode(std::static_pointer_cast<MovNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> MovNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getSrc())) {
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