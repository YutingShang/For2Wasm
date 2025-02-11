#include "DeclareNode.h"

DeclareNode::DeclareNode(std::string datatype, std::string var){
    if (!IRSemantics::isProgramVariable(var)) {
        throw std::runtime_error("Invalid variable name in DECLARE statement: " + var);
    }
    if (!IRSemantics::isValidDatatype(datatype)) {
        throw std::runtime_error("Invalid datatype in DECLARE statement: " + datatype);
    }
    this->datatype = datatype;
    this->var = var;
}

std::string DeclareNode::getText() const {
    return "DECLARE " + getDatatype() + " " + getVar();
}

std::shared_ptr<BaseNode> DeclareNode::cloneContent() const {
    return std::make_shared<DeclareNode>("INT", getVar());
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string DeclareNode::getVar() const {
    return var;
}

std::string DeclareNode::getDatatype() const {
    return datatype;
}

void DeclareNode::setVar(std::string var) {
    if (!IRSemantics::isProgramVariable(var)) {
        throw std::runtime_error("Invalid variable name in DECLARE statement: " + var);
    }
    this->var = var;
}

void DeclareNode::setDatatype(std::string datatype) {
    if (!IRSemantics::isValidDatatype(datatype)) {
        throw std::runtime_error("Invalid datatype in DECLARE statement: " + datatype);
    }
    this->datatype = datatype;
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