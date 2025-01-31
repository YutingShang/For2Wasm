#pragma once

#include "SimpleNode.h"

class TestNode : public SimpleNode {
public:
    TestNode(std::string var);
    ~TestNode() override = default;
    BaseNode* copyNodeOnly() const override;

    std::string getVar() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    void setVar(std::string var);
}; 