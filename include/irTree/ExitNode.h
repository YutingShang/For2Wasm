#pragma once
#include "SimpleNode.h"

// ExitNode is a class for nodes that represent the end of a program
class ExitNode : public SimpleNode
{
public:
    ExitNode(); // text is EXIT
    ~ExitNode() override = default;
    BaseNode* cloneContent() const override;
    
    std::string accept(IrBaseVisitor *visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
};