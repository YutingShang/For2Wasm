#pragma once
#include "SimpleNode.h"

// LogicOpNode is a class for nodes that represent logic operations, AND, OR, NOT
class LogicBinOpNode : public SimpleNode {
public:
    LogicBinOpNode(std::string op, std::string dest, std::string src1, std::string src2);
    ~LogicBinOpNode() override = default;

    std::string getOp() const;
    std::string getDest() const;
    std::string getSrc1() const;
    std::string getSrc2() const;

    std::string accept(IrBaseVisitor* visitor) override;
};