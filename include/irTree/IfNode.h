#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// IF condition thenLabel endLabel
///NOTE: see IfElseNode.h for the other case

class IfNode : public BaseNode {

    public:
    
        IfNode(std::string condition, std::string thenLabel, std::string endLabel);     
        ~IfNode() override = default;
        BaseNode* cloneContent() const override;

        void addChild(BaseNode* child) override;
        void addChildAtIndex(BaseNode* child, int index) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getReferencedExpressions() const override;

        BaseNode* removeCurrentNodeFromIRTree() override;

        //converts an IfNode to an IfElseNode, with empty else block
        ///WARNING: will delete the current IfNode from memory, need to ensure references are no longer pointing to it
        ///TODO: maybe convert references to the current IfNode to smart pointers too
        IfElseNode* convertToIfElseNode();
};