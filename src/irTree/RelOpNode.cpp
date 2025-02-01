#include "RelOpNode.h"

RelOpNode::RelOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    if (!isValidRelOp(op)) {
        throw std::runtime_error("Invalid relational operation: " + op);
    }
    if (!isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    if (isStringConstant(src1) || isStringConstant(src2)) {
        throw std::runtime_error("Invalid logical operation operands, cannot be string constants: " + src1 + " or " + src2);
    }
    this->textVector = {op, dest, src1, src2};  
}

bool RelOpNode::isValidRelOp(const std::string& op) const {
    return op == "EQ" || op == "NE" || op == "LT" || op == "LE" || op == "GT" || op == "GE";
}

BaseNode* RelOpNode::cloneContent() const {
    return new RelOpNode(getOp(), getDest(), getSrc1(), getSrc2());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

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

void RelOpNode::setOp(std::string op) {
    if (!isValidRelOp(op)) {
        throw std::runtime_error("Invalid relational operation: " + op);
    }
    textVector[0] = op;
}

void RelOpNode::setDest(std::string dest) {
    if (!isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    textVector[1] = dest;
}

void RelOpNode::setSrc1(std::string src1) {
    if (isStringConstant(src1)) {
        throw std::runtime_error("Invalid logical operation operands, cannot be string constants: " + src1);
    }
    textVector[2] = src1;
}

void RelOpNode::setSrc2(std::string src2) {
    if (isStringConstant(src2)) {
        throw std::runtime_error("Invalid logical operation operands, cannot be string constants: " + src2);
    }
    textVector[3] = src2;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string RelOpNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitRelOpNode(this);
}

/////////////////////////ANALYSIS METHODS/////////////////////////

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
    if (op == "EQ") return {getSrc1() + "==" + getSrc2()};
    if (op == "NE") return {getSrc1() + "!=" + getSrc2()};
    if (op == "LT") return {getSrc1() + "<" + getSrc2()};
    if (op == "LE") return {getSrc1() + "<=" + getSrc2()};
    if (op == "GT") return {getSrc1() + ">" + getSrc2()};
    if (op == "GE") return {getSrc1() + ">=" + getSrc2()};
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