#include "RelOpNode.h"

RelOpNode::RelOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    this->textVector = {op, dest, src1, src2};  
}

std::string RelOpNode::getOp() const {
    return textVector[0];
}

std::string RelOpNode::getDest() const {
    return textVector[1];
}

std::string RelOpNode::getSrc1() const {
    return textVector[2];
}

std::string RelOpNode::getSrc2() const {
    return textVector[3];
} 

std::string RelOpNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitRelOpNode(this);
}