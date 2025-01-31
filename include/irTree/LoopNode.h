#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// LOOP <body> <endloop>

class LoopNode : public BaseNode {

    public:
        LoopNode(std::string body, std::string endloop);
        
        ~LoopNode() override = default;
        
        BaseNode* copyNodeOnly() const override;

        void addChild(BaseNode* child) override;
        void addChildAtIndex(BaseNode* child, int index) override;

        std::string stringifyIRTree() const override;

        std::string accept(IrBaseVisitor* visitor) override;

        std::string getBodyLabel() const;
        std::string getEndloopLabel() const;

        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getReferencedExpressions() const override;

        BaseNode* removeCurrentNodeFromIRTree() override;

        //inserts a newNode node between the LoopNode node and its existing body child
        void insertSandwichBodyChild(SimpleNode* newNode);
};