#pragma once

#include "BaseNode.h"

// IF condition thenLabel elseLabel endLabel

class IfElseNode : public BaseNode {
public:
    IfElseNode(std::string condition, std::string thenLabel, std::string elseLabel, std::string endLabel);
    ~IfElseNode() override = default;

    void addChild(BaseNode* child) override;

    std::string stringifyIRTree() const override;

    std::string accept(IrBaseVisitor* visitor) override;
}; 