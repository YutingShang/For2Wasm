#pragma once

#include "SimpleNode.h"

//like the DATA statements in Fortran/wasm
//for initialising an array with constant values

class DataArrayNode : public SimpleNode {
public:
    //Constructor and destructor
    DataArrayNode(std::string arrayName, std::vector<std::string> dataValues);
    ~DataArrayNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getArrayVar() const;
    std::vector<std::string> getDataValues() const;
    void setArrayName(std::string arrayName);
    void setDataValues(std::vector<std::string> dataValues);

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

private:
    std::string arrayName;
    std::vector<std::string> dataValues;
    
};