#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// LOOP <loop body> <exit>

class LoopNode : public BaseNode {

    public:
        LoopNode(std::string body, std::string exit);

        ~LoopNode() override = default;

        void addChild(BaseNode* child) override;
        void insertChild(BaseNode* child, int index) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::string getBodyLabel() const;
        std::string getExitLabel() const;

        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getGeneratedExpressions() const override;

        BaseNode* removeCurrentNodeFromIRTree() override;
};