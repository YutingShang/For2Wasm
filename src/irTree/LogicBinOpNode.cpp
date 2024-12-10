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

std::string LogicBinOpNode::getSrc1() const {
    return textVector[2];
}

std::string LogicBinOpNode::getSrc2() const {
    return textVector[3];
} 

std::string LogicBinOpNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLogicBinOpNode(this);
}