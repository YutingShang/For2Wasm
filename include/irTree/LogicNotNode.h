#pragma once
#include "ExpressionNode.h"


// NOT <dest> <src>

class LogicNotNode : public ExpressionNode {
public:
    LogicNotNode(std::string dest, std::string src);
    ~LogicNotNode() override = default;
    BaseNode* copyNodeOnly() const override;

    std::string getDest() const override;
    void setDest(std::string dest) override;
    std::string getSrc() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    void setSrc(std::string src);
};