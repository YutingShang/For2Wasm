#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// LOOP <body> <endloop>

class LoopNode : public BaseNode {

    public:
        //Constructor and destructor
        LoopNode(std::string labelNumber);
        ~LoopNode() override = default;
        BaseNode* cloneContent() const override;
        std::string stringifyIRTree() const override;

        //Getters and setters
        std::string getBodyLabel() const;
        std::string getEndloopLabel() const;
        std::string getLabelNumber() const;

        //Visitor pattern
        std::string accept(IrBaseVisitor* visitor) override;

        //Analysis methods
        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getReferencedExpressions() const override;

        //Tree manipulation methods
        void addChild(BaseNode* child) override;
        void addChildAtIndex(BaseNode* child, int index) override;
        BaseNode* removeCurrentNodeFromIRTree() override;

        //inserts a newNode node between the LoopNode node and its existing body child
        void insertSandwichBodyChild(SimpleNode* newNode);

    private:
        std::string labelNumber;
};