#pragma once
#include "ExpressionNode.h"

// LogicOpNode is a class for nodes that represent logic operations taking two operands, AND, OR
// <op> <dest> <src1> <src2>
class LogicBinOpNode : public ExpressionNode {
public:
    //Constructor and destructor
    LogicBinOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~LogicBinOpNode() override = default;
    std::string getText() const override;
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
    std::string op;
    std::string dest;
    std::string src1;
    std::string src2;
    
};