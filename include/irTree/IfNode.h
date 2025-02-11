#pragma once

#include "BaseNode.h"
#include "EndBlockNode.h"

// IF condition thenLabel endLabel
///NOTE: see IfElseNode.h for the other case

class IfNode : public BaseNode {
public:
    //Constructor and destructor
    IfNode(std::string labelNumber);     
    ~IfNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;
    std::string stringifyIRTree() const override;

    //Getters and setters
    std::string getConditionLabel() const;
    std::string getThenLabel() const;
    std::string getEndLabel() const;
    std::string getLabelNumber() const;
    std::shared_ptr<BaseNode> getConditionNode() const;
    std::shared_ptr<BaseNode> getThenNode() const;
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

    //converts an IfNode to an IfElseNode, with empty else block
    ///WARNING: will delete the current IfNode from memory, need to ensure references are no longer pointing to it
    ///TODO: maybe convert references to the current IfNode to smart pointers too
    std::shared_ptr<IfElseNode> convertToIfElseNode();

private:
    std::string labelNumber;
    std::string conditionLabel;
    std::string thenLabel;
    std::string endLabel;
};