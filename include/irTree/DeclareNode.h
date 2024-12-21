#pragma once
#include "SimpleNode.h"

class DeclareNode : public SimpleNode {
public:
    DeclareNode(std::string var);
    ~DeclareNode() override = default;

    std::string getVar() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
};