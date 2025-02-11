#include "RelOpNode.h"

RelOpNode::RelOpNode(std::string op, std::string dest, std::string src1, std::string src2) {
    if (!IRSemantics::isValidRelOp(op)) {
        throw std::runtime_error("Invalid relational operation: " + op);
    }
    if (!IRSemantics::isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    if (IRSemantics::isStringConstant(src1) || IRSemantics::isStringConstant(src2)) {
        throw std::runtime_error("Invalid logical operation operands, cannot be string constants: " + src1 + " or " + src2);
    }
    this->op = op;
    this->dest = dest;
    this->src1 = src1;
    this->src2 = src2;  
}

std::string RelOpNode::getText() const {
    return op + " " + dest + " " + src1 + " " + src2;
}

std::shared_ptr<BaseNode> RelOpNode::cloneContent() const {
    return std::make_shared<RelOpNode>(getOp(), getDest(), getSrc1(), getSrc2());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string RelOpNode::getOp() const {
    return op;
}

std::string RelOpNode::getDest() const {
    return dest;
}

std::string RelOpNode::getSrc1() const {
    return src1;
}

std::string RelOpNode::getSrc2() const {
    return src2;
} 

void RelOpNode::setOp(std::string op) {
    if (!IRSemantics::isValidRelOp(op)) {
        throw std::runtime_error("Invalid relational operation: " + op);
    }
    this->op = op;
}

void RelOpNode::setDest(std::string dest) {
    if (!IRSemantics::isVariable(dest)) {
        throw std::runtime_error("Invalid destination variable: " + dest);
    }
    this->dest = dest;
}

void RelOpNode::setSrc1(std::string src1) {
    if (IRSemantics::isStringConstant(src1)) {
        throw std::runtime_error("Invalid logical operation operands, cannot be string constants: " + src1);
    }
    this->src1 = src1;
}

void RelOpNode::setSrc2(std::string src2) {
    if (IRSemantics::isStringConstant(src2)) {
        throw std::runtime_error("Invalid logical operation operands, cannot be string constants: " + src2);
    }
    this->src2 = src2;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string RelOpNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitRelOpNode(std::static_pointer_cast<RelOpNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> RelOpNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getSrc1())) {
        referencedVariables.insert(getSrc1());
    }
    if (IRSemantics::isVariable(getSrc2())) {
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