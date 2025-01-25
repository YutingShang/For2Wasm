#include "LogicBinOpNode.h"

LogicBinOpNode::LogicBinOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    this->textVector = {op, dest, src1, src2};
}

std::string LogicBinOpNode::getOp() const {
    return textVector[0];
}

std::string LogicBinOpNode::getDest() const {
    return textVector[1];
}

void LogicBinOpNode::setDest(std::string dest) {
    textVector[1] = dest;
}

std::string LogicBinOpNode::getSrc1() const {
    return textVector[2];
}

std::string LogicBinOpNode::getSrc2() const {
    return textVector[3];
} 

void LogicBinOpNode::setSrc1(std::string src1) {
    textVector[2] = src1;
}

void LogicBinOpNode::setSrc2(std::string src2) {
    textVector[3] = src2;
}

std::string LogicBinOpNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLogicBinOpNode(this);
}

std::set<std::string> LogicBinOpNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getSrc1())) {
        referencedVariables.insert(getSrc1());
    }
    if (isVariable(getSrc2())) {
        referencedVariables.insert(getSrc2());
    }
    return referencedVariables;
}

std::set<std::string> LogicBinOpNode::getDefinedVariables() const {
    return {getDest()};
}

std::set<std::string> LogicBinOpNode::getReferencedExpressions() const {
    std::string op = getOp();

    //e.g. if x = x|y, then we say x|y has been used/referenced by this node (NOTE: this is different to generated expressions)
    if (op == "OR") {
        return {getSrc1() + "|" + getSrc2()};
    } else if (op == "AND") {
        return {getSrc1() + "&" + getSrc2()};
    }
    throw std::runtime_error("Invalid logic binary operation: " + op);
}

void LogicBinOpNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc1() == oldVar) {
        setSrc1(newVar);
    }
    if (getSrc2() == oldVar) {
        setSrc2(newVar);
    }
}