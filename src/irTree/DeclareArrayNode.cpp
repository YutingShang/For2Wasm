#include "DeclareArrayNode.h"

DeclareArrayNode::DeclareArrayNode(std::string datatype, std::string arrayVar, std::vector<std::string> arrayDimensions): arrayDimensions(arrayDimensions){
    if (!IRSemantics::isProgramVariable(arrayVar)) {
        throw std::runtime_error("Invalid variable name: not a program variable that needs to be declared : " + arrayVar);
    }
    if (!IRSemantics::isValidDatatype(datatype)) {
        throw std::runtime_error("Invalid datatype in DECLARE ARRAY statement: " + datatype);
    }
    
    this->datatype = datatype;
    this->arrayVar = arrayVar;
    this->arrayDimensions = arrayDimensions;
}

std::string DeclareArrayNode::getText() const {
    return "DECLARE " + getDatatype() + " ARRAY " + getArrayVar() + " " + IRSemantics::getVectorText(getArrayDimensions());
}

std::shared_ptr<BaseNode> DeclareArrayNode::cloneContent() const {
    return std::make_shared<DeclareArrayNode>(getDatatype(), getArrayVar(), getArrayDimensions());
}


/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string DeclareArrayNode::getArrayVar() const {
    return arrayVar;
}

std::string DeclareArrayNode::getDatatype() const {
    return datatype;
}

std::vector<std::string> DeclareArrayNode::getArrayDimensions() const {
    return arrayDimensions;
}

void DeclareArrayNode::setArrayVar(std::string arrayVar) {
    if (!IRSemantics::isProgramVariable(arrayVar)) {
        throw std::runtime_error("Invalid variable name: not a program variable that needs to be declared : " + arrayVar);
    }
    this->arrayVar = arrayVar;
}

void DeclareArrayNode::setDatatype(std::string datatype) {
    if (!IRSemantics::isValidDatatype(datatype)) {
        throw std::runtime_error("Invalid datatype in DECLARE ARRAY statement: " + datatype);
    }
    this->datatype = datatype;
}

void DeclareArrayNode::setArrayDimensions(std::vector<std::string> arrayDimensions) {
    this->arrayDimensions = arrayDimensions;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string DeclareArrayNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitDeclareArrayNode(std::static_pointer_cast<DeclareArrayNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> DeclareArrayNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> DeclareArrayNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> DeclareArrayNode::getReferencedExpressions() const {
    return {};
}