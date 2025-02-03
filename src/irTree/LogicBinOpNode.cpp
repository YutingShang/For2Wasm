#include "LogicBinOpNode.h"

LogicBinOpNode::LogicBinOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    if (!isValidLogicBinOp(op)) {
        throw std::runtime_error("Invalid logic binary operation: " + op);
    }
    if (!isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    if (isStringConstant(src1) || isStringConstant(src2)) {
        throw std::runtime_error("Invalid logic binary operation operands, cannot be string constants: " + src1 + " or " + src2);
    }
    this->textVector = {op, dest, src1, src2};
}

std::shared_ptr<BaseNode> LogicBinOpNode::cloneContent() const {
    return std::make_shared<LogicBinOpNode>(getOp(), getDest(), getSrc1(), getSrc2());
}

bool LogicBinOpNode::isValidLogicBinOp(const std::string& op) const {
    return op == "OR" || op == "AND";
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LogicBinOpNode::getOp() const {
    return textVector[0];
}

std::string LogicBinOpNode::getDest() const {
    return textVector[1];
}

std::string LogicBinOpNode::getSrc1() const {
    return textVector[2];
}

std::string LogicBinOpNode::getSrc2() const {
    return textVector[3];
} 

void LogicBinOpNode::setOp(std::string op) {
    if (!isValidLogicBinOp(op)) {
        throw std::runtime_error("Invalid logic binary operation: " + op);
    }
    textVector[0] = op;
}

void LogicBinOpNode::setDest(std::string dest) {
    if (!isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    textVector[1] = dest;
}

void LogicBinOpNode::setSrc1(std::string src1) {
    if (isStringConstant(src1)) {
        throw std::runtime_error("Invalid logic binary operation operands, cannot be string constants: " + src1);
    }
    textVector[2] = src1;
}

void LogicBinOpNode::setSrc2(std::string src2) {
    if (isStringConstant(src2)) {
        throw std::runtime_error("Invalid logic binary operation operands, cannot be string constants: " + src2);
    }
    textVector[3] = src2;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LogicBinOpNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitLogicBinOpNode(std::static_pointer_cast<LogicBinOpNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

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