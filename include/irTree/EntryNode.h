#pragma once

#include "SimpleNode.h"


//similar to ExitNode, has no body 

class EntryNode : public SimpleNode {
public:
    EntryNode();
    ~EntryNode() override = default;
    BaseNode* copyNodeOnly() const override;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
}; 