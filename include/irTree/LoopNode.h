#pragma once

#include "BaseNode.h"

class LoopNode : public BaseNode {

    public:
        LoopNode(std::string body, std::string exit);

        ~LoopNode() override = default;

        void addChild(BaseNode* child) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::string getBodyLabel() const;
        std::string getExitLabel() const;

};