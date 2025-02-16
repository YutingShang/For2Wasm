#pragma once
#include "SimpleNode.h"

class DeclareArrayNode : public SimpleNode {
public:
    //Constructor and destructor
    DeclareArrayNode(std::string datatype, std::string arrayVar, std::vector<std::string> arrayDimensions);
    ~DeclareArrayNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getArrayVar() const;
    std::string getDatatype() const;
    std::vector<int> getArrayDimensions() const;
    void setArrayVar(std::string arrayVar);
    void setDatatype(std::string datatype);
    

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

private:
    std::string arrayVar;
    std::string datatype;
    std::vector<int> arrayDimensions;    //actual dimensions of the array, vector easy for processing - to print, use getArrayDimensionsText
    
};