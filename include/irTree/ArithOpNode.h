#pragma once
#include "SimpleNode.h"

class ArithOpNode : public SimpleNode {
public:
    ArithOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~ArithOpNode() override = default;

    std::string getOp() const;
    std::string getDest() const;
    std::string getSrc1() const;
    std::string getSrc2() const;

    std::string accept(IrBaseVisitor* visitor) override;
};