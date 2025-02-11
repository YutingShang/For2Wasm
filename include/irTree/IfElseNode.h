#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// IF condition thenLabel elseLabel endLabel

class IfElseNode : public BaseNode {
public:
    //Constructor and destructor
    IfElseNode(std::string labelNumber);
    ~IfElseNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;
    std::string stringifyIRTree() const override;

    //Getters and setters
    std::string getConditionLabel() const;
    std::string getThenLabel() const;
    std::string getElseLabel() const;
    std::string getEndLabel() const;
    std::string getLabelNumber() const;
    //getting the Nodes
    std::shared_ptr<BaseNode> getConditionNode() const;
    std::shared_ptr<BaseNode> getThenNode() const;
    std::shared_ptr<BaseNode> getElseNode() const;
    std::shared_ptr<BaseNode> getEndIfNode() const;

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
    std::string conditionLabel;
    std::string thenLabel;
    std::string elseLabel;
    std::string endLabel;
}; 