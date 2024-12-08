#pragma once

#include "BaseNode.h"

class IfNode : public BaseNode {

    public:
        IfNode(std::string text);
        
        ~IfNode() override = default;

        void addChild(BaseNode* child) override;

        std::string stringifyIRTree() const override;

};