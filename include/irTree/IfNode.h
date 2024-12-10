#pragma once

#include "BaseNode.h"

class IfNode : public BaseNode {

    public:
        ///NOTE:IF node varies in constructor, so you should provide the whole instruction when creating
        // e.g. IF condLabel thenLabel elseLabel endLabel
        // or IF condLabel thenLabel endLabel
        IfNode(std::string text);     
        ~IfNode() override = default;

        void addChild(BaseNode* child) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

};