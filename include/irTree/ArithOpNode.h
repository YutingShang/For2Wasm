#pragma once
#include "SimpleNode.h"

// Arithmetic operations
// <op> <dest> <src1> <src2>
// where <op> is one of ADD, SUB, MUL, DIV

class ArithOpNode : public SimpleNode {
public:
    ArithOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~ArithOpNode() override = default;

    std::string getOp() const;
    std::string getDest() const;
    std::string getSrc1() const;
    std::string getSrc2() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
};