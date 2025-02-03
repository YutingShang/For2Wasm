#pragma once
#include "SimpleNode.h"

class DeclareNode : public SimpleNode {
public:
    //Constructor and destructor
    DeclareNode(std::string var);
    ~DeclareNode() override = default;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getVar() const;
    void setVar(std::string var);

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
};