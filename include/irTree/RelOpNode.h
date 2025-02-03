#pragma once
#include "ExpressionNode.h"

// RelOpNode is a class for nodes that represent relational operations
// EQ, NE, LT, LE, GT, GE
class RelOpNode : public ExpressionNode {
public:
    //Constructor and destructor
    RelOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~RelOpNode() override = default;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getOp() const;
    std::string getDest() const override;
    std::string getSrc1() const;
    std::string getSrc2() const;
    void setOp(std::string op);
    void setDest(std::string dest) override;
    void setSrc1(std::string src1);
    void setSrc2(std::string src2);

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    bool isValidRelOp(const std::string& op) const;
};