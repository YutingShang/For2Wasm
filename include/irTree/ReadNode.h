#pragma once

#include "SimpleNode.h"

class ReadNode : public SimpleNode {
public:
    ReadNode(std::string var);
    ~ReadNode() override = default;

    std::string getVar() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getGeneratedExpressions() const override;
};