#include "RelOpNode.h"

RelOpNode::RelOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    this->textVector = {op, dest, src1, src2};  
}

BaseNode* RelOpNode::copyNodeOnly() const {
    return new RelOpNode(textVector[0], textVector[1], textVector[2], textVector[3]);
}

std::string RelOpNode::getOp() const {
    return textVector[0];
}

std::string RelOpNode::getDest() const {
    return textVector[1];
}

void RelOpNode::setDest(std::string dest) {
    textVector[1] = dest;
}

std::string RelOpNode::getSrc1() const {
    return textVector[2];
}

std::string RelOpNode::getSrc2() const {
    return textVector[3];
} 

void RelOpNode::setSrc1(std::string src1) {
    textVector[2] = src1;
}

void RelOpNode::setSrc2(std::string src2) {
    textVector[3] = src2;
}

std::string RelOpNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitRelOpNode(this);
}

std::set<std::string> RelOpNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (isVariable(getSrc1())) {
        referencedVariables.insert(getSrc1());
    }
    if (isVariable(getSrc2())) {
        referencedVariables.insert(getSrc2());
    }
    return referencedVariables;
}

std::set<std::string> RelOpNode::getDefinedVariables() const {
    return {getDest()};
}

std::set<std::string> RelOpNode::getReferencedExpressions() const {
    std::string op = getOp();

    //e.g. if x = x==y, then we say x==y has been used/referenced by this node (NOTE: this is different to generated expressions)
    if (op == "EQ") {
        return {getSrc1() + "==" + getSrc2()};
    }else if (op == "NE") {
        return {getSrc1() + "!=" + getSrc2()};
    }else if (op == "LT") {
        return {getSrc1() + "<" + getSrc2()};
    }else if (op == "LE") {
        return {getSrc1() + "<=" + getSrc2()};
    }else if (op == "GT") {
        return {getSrc1() + ">" + getSrc2()};
    }else if (op == "GE") {
        return {getSrc1() + ">=" + getSrc2()};
    }
    throw std::runtime_error("Invalid relational operation: " + op);
}

void RelOpNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc1() == oldVar) {
        setSrc1(newVar);
    }
    if (getSrc2() == oldVar) {
        setSrc2(newVar);
    }
}