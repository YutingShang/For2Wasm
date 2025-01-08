#pragma once
#include "SimpleNode.h"

//an abstract class for ArithOpNode, LogicBinOpNode, LogicNotNode and RelOpNode
//always has a 'dest' member variable, which a setter function will be provided for to change the dest
///UNSURE: what about MovNode?

class ExpressionNode : public SimpleNode {
public:
    ExpressionNode();
    ~ExpressionNode() override = default;

    virtual std::string getDest() const = 0;
    virtual void setDest(std::string dest) = 0;
};