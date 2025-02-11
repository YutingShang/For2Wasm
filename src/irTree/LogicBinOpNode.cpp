#include "LogicBinOpNode.h"

LogicBinOpNode::LogicBinOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    if (!IRSemantics::isValidLogicBinOp(op)) {
        throw std::runtime_error("Invalid logic binary operation: " + op);
    }
    if (!IRSemantics::isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    if (IRSemantics::isStringConstant(src1) || IRSemantics::isStringConstant(src2)) {
        throw std::runtime_error("Invalid logic binary operation operands, cannot be string constants: " + src1 + " or " + src2);
    }
    this->op = op;
    this->dest = dest;
    this->src1 = src1;
    this->src2 = src2;
}

std::string LogicBinOpNode::getText() const {
    return op + " " + dest + " " + src1 + " " + src2;
}

std::shared_ptr<BaseNode> LogicBinOpNode::cloneContent() const {
    return std::make_shared<LogicBinOpNode>(getOp(), getDest(), getSrc1(), getSrc2());
}


/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LogicBinOpNode::getOp() const {
    return op;
}

std::string LogicBinOpNode::getDest() const {
    return dest;
}

std::string LogicBinOpNode::getSrc1() const {
    return src1;
}

std::string LogicBinOpNode::getSrc2() const {
    return src2;
} 

void LogicBinOpNode::setOp(std::string op) {
    if (!IRSemantics::isValidLogicBinOp(op)) {
        throw std::runtime_error("Invalid logic binary operation: " + op);
    }
    this->op = op;
}

void LogicBinOpNode::setDest(std::string dest) {
    if (!IRSemantics::isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    this->dest = dest;
}

void LogicBinOpNode::setSrc1(std::string src1) {
    if (IRSemantics::isStringConstant(src1)) {
        throw std::runtime_error("Invalid logic binary operation operands, cannot be string constants: " + src1);
    }
    this->src1 = src1;
}

void LogicBinOpNode::setSrc2(std::string src2) {
    if (IRSemantics::isStringConstant(src2)) {
        throw std::runtime_error("Invalid logic binary operation operands, cannot be string constants: " + src2);
    }
    this->src2 = src2;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LogicBinOpNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitLogicBinOpNode(std::static_pointer_cast<LogicBinOpNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> LogicBinOpNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getSrc1())) {
        referencedVariables.insert(getSrc1());
    }
    if (IRSemantics::isVariable(getSrc2())) {
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