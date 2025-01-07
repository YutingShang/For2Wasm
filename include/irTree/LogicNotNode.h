#pragma once
#include "SimpleNode.h"


// NOT <dest> <src>

class LogicNotNode : public SimpleNode {
public:
    LogicNotNode(std::string dest, std::string src);
    ~LogicNotNode() override = default;

    std::string getDest() const;
    std::string getSrc() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getGeneratedExpressions() const override;
};