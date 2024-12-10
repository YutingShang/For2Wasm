#pragma once
#include "SimpleNode.h"

// RelOpNode is a class for nodes that represent relational operations
// EQ, NE, LT, LE, GT, GE
class RelOpNode : public SimpleNode {
public:
    RelOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~RelOpNode() override = default;

    std::string getOp() const;
    std::string getDest() const;
    std::string getSrc1() const;
    std::string getSrc2() const;

    std::string accept(IrBaseVisitor* visitor) override;
};