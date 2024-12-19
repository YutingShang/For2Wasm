#pragma once

#include "SimpleNode.h"

class TestNode : public SimpleNode {
public:
    TestNode(std::string var);
    ~TestNode() override = default;

    std::string getVar() const;

    std::string accept(IrBaseVisitor* visitor) override;
}; 