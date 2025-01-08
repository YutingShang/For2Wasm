#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// IF condition thenLabel endLabel
///NOTE: see IfElseNode.h for the other case

class IfNode : public BaseNode {

    public:
    
        IfNode(std::string condition, std::string thenLabel, std::string endLabel);     
        ~IfNode() override = default;

        void addChild(BaseNode* child) override;
        void addChildAtIndex(BaseNode* child, int index) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getGeneratedExpressions() const override;

        BaseNode* removeCurrentNodeFromIRTree() override;
};