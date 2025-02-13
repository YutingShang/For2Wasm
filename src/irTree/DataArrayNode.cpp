#include "DataArrayNode.h"

DataArrayNode::DataArrayNode(std::string arrayName, std::vector<std::string> dataValues): arrayName(arrayName), dataValues(dataValues){
    if (!IRSemantics::isProgramVariable(arrayName)) {
        throw std::runtime_error("Invalid variable name: not a program variable that needs to be initialised : " + arrayName);
    }
}

std::string DataArrayNode::getText() const {
    return "DATA " + arrayName + " " + IRSemantics::getVectorText(dataValues);
}

std::shared_ptr<BaseNode> DataArrayNode::cloneContent() const {
    return std::make_shared<DataArrayNode>(arrayName, dataValues);
}


/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string DataArrayNode::getArrayName() const {
    return arrayName;
}

std::vector<std::string> DataArrayNode::getDataValues() const {
    return dataValues;
}

void DataArrayNode::setArrayName(std::string arrayName) {
    if (!IRSemantics::isProgramVariable(arrayName)) {
        throw std::runtime_error("Invalid variable name: not a program variable that needs to be initialised : " + arrayName);
    }
    this->arrayName = arrayName;
}

void DataArrayNode::setDataValues(std::vector<std::string> dataValues) {
    this->dataValues = dataValues;
}


/////////////////////////VISITOR PATTERN/////////////////////////

std::string DataArrayNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitDataArrayNode(std::static_pointer_cast<DataArrayNode>(shared_from_this()));
}


/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> DataArrayNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> DataArrayNode::getDefinedVariables() const {
    return {arrayName};
}

std::set<std::string> DataArrayNode::getReferencedExpressions() const {
    return {};
}



