#pragma once
#include "SimpleNode.h"

// ExitNode is a class for nodes that represent the end of a program
class ExitNode : public SimpleNode
{
public:
    //Constructor and destructor
    ExitNode() = default;
    ~ExitNode() override = default;
    std::string getText() const override;       // text is EXIT
    std::shared_ptr<BaseNode> cloneContent() const override;
    
    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
};