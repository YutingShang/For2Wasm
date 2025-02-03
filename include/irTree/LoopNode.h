#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// LOOP <body> <endloop>

class LoopNode : public BaseNode {

    public:
        //Constructor and destructor
        LoopNode(std::string labelNumber);
        ~LoopNode() override = default;
        std::shared_ptr<BaseNode> cloneContent() const override;
        std::string stringifyIRTree() const override;

        //Getters and setters
        std::string getBodyLabel() const;
        std::string getEndloopLabel() const;
        std::string getLabelNumber() const;
        std::shared_ptr<BaseNode> getBodyNode() const;
        std::shared_ptr<BaseNode> getEndLoopNode() const;

        //Visitor pattern
        std::string accept(IrBaseVisitor& visitor) override;

        //Analysis methods
        std::set<std::string> getReferencedVariables() const override;
        std::set<std::string> getDefinedVariables() const override;
        std::set<std::string> getReferencedExpressions() const override;

        //Tree manipulation methods
        void addChild(std::shared_ptr<BaseNode> child) override;
        void addChildAtIndex(std::shared_ptr<BaseNode> child, int index) override;
        std::shared_ptr<BaseNode> removeCurrentNodeFromIRTree() override;

        //inserts a newNode node between the LoopNode node and its existing body child
        void insertSandwichBodyChild(std::shared_ptr<SimpleNode> newNode);

    private:
        std::string labelNumber;
};