#pragma once

#include "SimpleNode.h"

class ReadNode : public SimpleNode {
public:
    ReadNode(std::string var);
    ~ReadNode() override = default;

    std::string getVar() const;

    std::string accept(IrBaseVisitor* visitor) override;
};