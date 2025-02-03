#pragma once
#include "ExpressionNode.h"


// NOT <dest> <src>

class LogicNotNode : public ExpressionNode {
public:
    //Constructor and destructor
    LogicNotNode(std::string dest, std::string src);
    ~LogicNotNode() override = default;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getDest() const override;
    std::string getSrc() const;
    void setDest(std::string dest) override;
    void setSrc(std::string src);

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

};