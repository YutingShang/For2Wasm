#pragma once
#include "ExpressionNode.h"

// Arithmetic operations
// <op> <dest> <src1> <src2>
// where <op> is one of ADD, SUB, MUL, DIV

class ArithOpNode : public ExpressionNode {
public:
    // Constructor and destructor
    ArithOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~ArithOpNode() override = default;
    BaseNode* cloneContent() const override;

    // Getters and setters for each component
    std::string getOp() const;
    std::string getDest() const override;     //override from ExpressionNode
    std::string getSrc1() const;
    std::string getSrc2() const;
    
    void setOp(std::string op);
    void setDest(std::string dest) override;
    void setSrc1(std::string src1);
    void setSrc2(std::string src2);

    // Visitor pattern
    std::string accept(IrBaseVisitor* visitor) override;

    // Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    // Helper methods for validation
    bool isValidArithOp(const std::string& op) const;
};