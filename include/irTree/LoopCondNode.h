#pragma once

#include "BaseNode.h"


// LOOP <termination condition> <loop body> <step> <endloop>
class LoopCondNode : public BaseNode {
    public:
        LoopCondNode(std::string cond, std::string body, std::string step, std::string endloop);

        ~LoopCondNode() override = default;

        void addChild(BaseNode* child) override;
        void insertChild(BaseNode* child, int index) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::string getBodyLabel() const;
        std::string getEndLoopLabel() const;

        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;

};