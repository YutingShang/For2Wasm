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
        BaseNode* cloneContent() const override;
        std::string stringifyIRTree() const override;

        //Getters and setters
        std::string getInitLabel() const;
        std::string getCondLabel() const;
        std::string getBodyLabel() const;
        std::string getStepLabel() const;
        std::string getEndLoopLabel() const;
        std::string getLabelNumber() const;   //the label for each of the cond, body, step, endloop

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

    private:
        std::string labelNumber;
};