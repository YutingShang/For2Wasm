#pragma once
#include "SimpleNode.h"

class LogicNotNode : public SimpleNode {
public:
    LogicNotNode(std::string dest, std::string src);
    ~LogicNotNode() override = default;

    std::string getDest() const;
    std::string getSrc() const;

    std::string accept(IrBaseVisitor* visitor) override;
};