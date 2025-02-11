#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"


// LOOP <initialiser> <termination condition> <body> <step> <endloop>
// i.e. LOOP <init> <cond> <body> <step> <endloop>

class LoopCondNode : public BaseNode {
    public:
        //Constructor and destructor
        LoopCondNode(std::string labelNumber);
        ~LoopCondNode() override = default;
        std::string getText() const override;
        std::shared_ptr<BaseNode> cloneContent() const override;
        std::string stringifyIRTree() const override;

        //Getters and setters
        std::string getInitLabel() const;
        std::string getCondLabel() const;
        std::string getBodyLabel() const;
        std::string getStepLabel() const;
        std::string getEndLoopLabel() const;
        std::string getLabelNumber() const;   //the label for each of the cond, body, step, endloop
        std::shared_ptr<BaseNode> getInitNode() const;
        std::shared_ptr<BaseNode> getCondNode() const;
        std::shared_ptr<BaseNode> getBodyNode() const;
        std::shared_ptr<BaseNode> getStepNode() const;
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

    private:
        std::string labelNumber;
        std::string initLabel;
        std::string condLabel;
        std::string bodyLabel;
        std::string stepLabel;
        std::string endLoopLabel;
};