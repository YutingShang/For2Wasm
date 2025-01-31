#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// IF condition thenLabel elseLabel endLabel

class IfElseNode : public BaseNode {
public:
    IfElseNode(std::string condition, std::string thenLabel, std::string elseLabel, std::string endLabel);
    ~IfElseNode() override = default;
    BaseNode* copyNodeOnly() const override;

    void addChild(BaseNode* child) override;
    void addChildAtIndex(BaseNode* child, int index) override;

    std::string stringifyIRTree() const override;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

    BaseNode* removeCurrentNodeFromIRTree() override;
}; 