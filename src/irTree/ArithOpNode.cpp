#include "ArithOpNode.h"

ArithOpNode::ArithOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    if (op != "ADD" && op != "SUB" && op != "MUL" && op != "DIV") {
        throw std::runtime_error("Invalid arithmetic operation: " + op);
    }
    if (src1.at(0) == '$' || src2.at(0) == '$') {
        throw std::runtime_error("Invalid arithmetic operands, cannot be string constants: " + src1 + " or " + src2);
    }

    this->textVector = {op, dest, src1, src2};
}

BaseNode* ArithOpNode::cloneContent() const {
    return new ArithOpNode(textVector[0], textVector[1], textVector[2], textVector[3]);
}

std::string ArithOpNode::getOp() const {
    return textVector[0];
}

std::string ArithOpNode::getDest() const {
    return textVector[1];
}

void ArithOpNode::setDest(std::string dest) {
    textVector[1] = dest;
}

std::string ArithOpNode::getSrc1() const {
    return textVector[2];
}

std::string ArithOpNode::getSrc2() const {
    return textVector[3];
} 

void ArithOpNode::setSrc1(std::string src1) {
    textVector[2] = src1;
}

void ArithOpNode::setSrc2(std::string src2) {
    textVector[3] = src2;
}

std::string ArithOpNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitArithOpNode(this);
}

std::set<std::string> ArithOpNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getSrc1())) {
        referencedVariables.insert(getSrc1());
    }
    if (isVariable(getSrc2())) {
        referencedVariables.insert(getSrc2());
    }
    return referencedVariables;
}

std::set<std::string> ArithOpNode::getDefinedVariables() const {
    return {getDest()};
}

std::set<std::string> ArithOpNode::getReferencedExpressions() const {
    std::string op = getOp();
    //the sources will either be user variables/temp variables or pos integer constants (no need to check for strings $ as they won't appear in the sources)
    //valid expression can be 'a+b' or 'a+3'

    //e.g. if x = x+y, then we say x+y has not been used/referenced by this node (NOTE: this is different to generated expressions)
    if (op == "ADD"){
        return {getSrc1() + "+" + getSrc2()};
    }else if (op == "SUB"){
        return {getSrc1() + "-" + getSrc2()};
    }else if (op == "MUL"){
        return {getSrc1() + "*" + getSrc2()};
    }else if (op == "DIV"){
        return {getSrc1() + "/" + getSrc2()};
    }
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