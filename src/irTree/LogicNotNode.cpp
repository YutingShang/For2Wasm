#include "LogicNotNode.h"

LogicNotNode::LogicNotNode(std::string dest, std::string src) {
    if (!IRSemantics::isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    if (IRSemantics::isStringConstant(src)) {
        throw std::runtime_error("Invalid logic NOT operation operand, cannot be string constant: " + src);
    }
    this->dest = dest;
    this->src = src;
}

std::string LogicNotNode::getText() const {
    return "NOT " + dest + " " + src;
}

std::shared_ptr<BaseNode> LogicNotNode::cloneContent() const {
    return std::make_shared<LogicNotNode>(getDest(), getSrc());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LogicNotNode::getDest() const {
    return dest;
}

std::string LogicNotNode::getSrc() const {
    return src;
}

void LogicNotNode::setDest(std::string dest) {
    this->dest = dest;
}

void LogicNotNode::setSrc(std::string src) {
    this->src = src;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LogicNotNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitLogicNotNode(std::static_pointer_cast<LogicNotNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> LogicNotNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getSrc())) {
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