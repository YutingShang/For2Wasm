#pragma once

#include "BaseNode.h"

class LoopNode : public BaseNode {

    public:
        LoopNode(std::string text);

        ~LoopNode() override = default;

        void addChild(BaseNode* child) override;

        std::string stringifyIRTree() const override;

};