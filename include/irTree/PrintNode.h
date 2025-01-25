#pragma once

#include "SimpleNode.h"

class PrintNode : public SimpleNode {
public:
    PrintNode(std::string src);
    ~PrintNode() override = default;

    std::string getSrc() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    void setSrc(std::string src);
};