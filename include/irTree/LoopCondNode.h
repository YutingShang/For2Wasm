#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"


// LOOP <initialiser> <termination condition> <body> <step> <endloop>
class LoopCondNode : public BaseNode {
    public:
        LoopCondNode(std::string init, std::string cond, std::string body, std::string step, std::string endloop);
        ~LoopCondNode() override = default;
        
        BaseNode* cloneContent() const override;

        void addChild(BaseNode* child) override;
        void addChildAtIndex(BaseNode* child, int index) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::string getBodyLabel() const;
        std::string getEndLoopLabel() const;

        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getReferencedExpressions() const override;

        BaseNode* removeCurrentNodeFromIRTree() override;
};