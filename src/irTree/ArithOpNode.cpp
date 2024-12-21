#include "ArithOpNode.h"

ArithOpNode::ArithOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    this->textVector = {op, dest, src1, src2};
}

std::string ArithOpNode::getOp() const {
    return textVector[0];
}

std::string ArithOpNode::getDest() const {
    return textVector[1];
}

std::string ArithOpNode::getSrc1() const {
    return textVector[2];
}

std::string ArithOpNode::getSrc2() const {
    return textVector[3];
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