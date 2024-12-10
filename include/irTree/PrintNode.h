#pragma once

#include "SimpleNode.h"

class PrintNode : public SimpleNode {
public:
    PrintNode(std::string src);
    ~PrintNode() override = default;

    std::string getSrc() const;

    std::string accept(IrBaseVisitor* visitor) override;
};