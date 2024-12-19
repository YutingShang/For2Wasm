#pragma once

#include "SimpleNode.h"


//similar to ExitNode, has no body 

class EntryNode : public SimpleNode {
public:
    EntryNode();
    ~EntryNode() override = default;

    std::string accept(IrBaseVisitor* visitor) override;
}; 