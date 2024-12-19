#pragma once

#include "BaseNode.h"

// IF condition thenLabel endLabel
///NOTE: see IfElseNode.h for the other case

class IfNode : public BaseNode {

    public:
    
        IfNode(std::string condition, std::string thenLabel, std::string endLabel);     
        ~IfNode() override = default;

        void addChild(BaseNode* child) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

};