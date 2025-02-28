#include "ArithOpNode.h"

ArithOpNode::ArithOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    if (!IRSemantics::isValidArithOp(op)) {
        throw std::runtime_error("Invalid arithmetic operation: " + op);
    }
    if (IRSemantics::isStringConstant(src1) || IRSemantics::isStringConstant(src2)) {
        throw std::runtime_error("Invalid arithmetic operands, cannot be string constants: " + src1 + " or " + src2);
    }
    if (!IRSemantics::isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }

    this->op = op;
    this->dest = dest;
    this->src1 = src1;
    this->src2 = src2;
}

std::string ArithOpNode::getText() const {
    return op + " " + dest + " " + src1 + " " + src2;
}


std::shared_ptr<BaseNode> ArithOpNode::cloneContent() const {
    return std::make_shared<ArithOpNode>(getOp(), getDest(), getSrc1(), getSrc2());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string ArithOpNode::getOp() const {
    return op;
}

std::string ArithOpNode::getDest() const {
    return dest;
}

std::string ArithOpNode::getSrc1() const {
    return src1;
}

std::string ArithOpNode::getSrc2() const {
    return src2;
}

void ArithOpNode::setOp(std::string op) {
    if (!IRSemantics::isValidArithOp(op)) {
        throw std::runtime_error("Invalid arithmetic operation: " + op);
    }
    this->op = op;
}

void ArithOpNode::setDest(std::string dest) {
    this->dest = dest;
}

void ArithOpNode::setSrc1(std::string src1) {
    if (IRSemantics::isStringConstant(src1)) {
        throw std::runtime_error("Invalid arithmetic operand, cannot be string constant: " + src1);
    }
    this->src1 = src1;
}

void ArithOpNode::setSrc2(std::string src2) {
    if (IRSemantics::isStringConstant(src2)) {
        throw std::runtime_error("Invalid arithmetic operand, cannot be string constant: " + src2);
    }
    this->src2 = src2;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string ArithOpNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitArithOpNode(std::static_pointer_cast<ArithOpNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> ArithOpNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getSrc1())) {
        referencedVariables.insert(getSrc1());
    }
    if (IRSemantics::isVariable(getSrc2())) {
        referencedVariables.insert(getSrc2());
    }
    return referencedVariables;
}

std::set<std::string> ArithOpNode::getDefinedVariables() const {
    return {getDest()};
}

std::set<std::string> ArithOpNode::getReferencedExpressions() const {
    //the sources will either be user variables/temp variables or pos integer constants (no need to check for strings $ as they won't appear in the sources)
    //valid expression can be 'a+b' or 'a+3'

    //e.g. if x = x+y, then we say x+y has not been used/referenced by this node (NOTE: this is different to generated expressions)
    const std::string op = getOp();
    const std::string src1 = getSrc1();
    const std::string src2 = getSrc2();

    if (op == "ADD") return {src1 + "+" + src2};
    if (op == "SUB") return {src1 + "-" + src2};
    if (op == "MUL") return {src1 + "*" + src2};
    if (op == "DIV") return {src1 + "/" + src2};
    if (op == "POW") return {src1 + "^" + src2};
    
    throw std::runtime_error("Invalid arithmetic operation: " + op);
}

void ArithOpNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc1() == oldVar) {
        setSrc1(newVar);
    }
    if (getSrc2() == oldVar) {
        setSrc2(newVar);
    }
}