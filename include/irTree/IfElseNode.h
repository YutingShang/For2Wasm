#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// IF condition thenLabel elseLabel endLabel

class IfElseNode : public BaseNode {
public:
    //Constructor and destructor
    IfElseNode(std::string labelNumber);
    ~IfElseNode() override = default;
    BaseNode* cloneContent() const override;
    std::string stringifyIRTree() const override;

    //Getters and setters
    std::string getConditionLabel() const;
    std::string getThenLabel() const;
    std::string getElseLabel() const;
    std::string getEndLabel() const;
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

private:
    std::string labelNumber;
}; 