#pragma once
#include "SimpleNode.h"

// ExitNode is a class for nodes that represent the end of a program
class ExitNode : public SimpleNode
{
public:
    ExitNode(); // text is EXIT
    ~ExitNode() override = default;

    std::string accept(IrBaseVisitor *visitor) override;
};