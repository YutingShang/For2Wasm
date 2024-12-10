#pragma once
#include "SimpleNode.h"

class DeclareNode : public SimpleNode {
public:
    DeclareNode(std::string var);
    ~DeclareNode() override = default;

    std::string getVar() const;

    std::string accept(IrBaseVisitor* visitor) override;
};