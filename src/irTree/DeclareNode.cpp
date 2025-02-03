#include "DeclareNode.h"

DeclareNode::DeclareNode(std::string var){
    if (isStringConstant(var)) {
        throw std::runtime_error("Invalid variable name: cannot be a string constant");
    }
    this->textVector = {"DECLARE", var};
}

std::shared_ptr<BaseNode> DeclareNode::cloneContent() const {
    return std::make_shared<DeclareNode>(getVar());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string DeclareNode::getVar() const {
    return textVector[1];
}

void DeclareNode::setVar(std::string var) {
    if (isStringConstant(var)) {
        throw std::runtime_error("Invalid variable name: cannot be a string constant");
    }
    textVector[1] = var;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string DeclareNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitDeclareNode(std::static_pointer_cast<DeclareNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> DeclareNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> DeclareNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> DeclareNode::getReferencedExpressions() const {
    return {};
}