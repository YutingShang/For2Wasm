#include "PrintNode.h"

PrintNode::PrintNode(std::string operand) {
    //operand can be a variable, integer, or string
    this->src = operand;
}

std::string PrintNode::getText() const {
    return "CALL PRINT " + src;
}

std::shared_ptr<BaseNode> PrintNode::cloneContent() const {
    return std::make_shared<PrintNode>(getSrc());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string PrintNode::getSrc() const {
    return src;
}

void PrintNode::setSrc(std::string src) {
    this->src = src;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string PrintNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitPrintNode(std::static_pointer_cast<PrintNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> PrintNode::getReferencedVariables() const {
    std::set<std::string> referencedVariables;
    if (IRSemantics::isVariable(getSrc())) {
        referencedVariables.insert(getSrc());
    }
    return referencedVariables;
}

std::set<std::string> PrintNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> PrintNode::getReferencedExpressions() const {
    return {};
}

void PrintNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    if (getSrc() == oldVar) {
        setSrc(newVar);
    }
}