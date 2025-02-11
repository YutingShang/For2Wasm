#pragma once
#include "SimpleNode.h"

//an abstract class for ArithOpNode, LogicBinOpNode, LogicNotNode and RelOpNode
//always has a 'dest' member variable, which a setter function will be provided for to change the dest
///NOTE: does not include MovNode, since it does not generate any 'expressions'

class ExpressionNode : public SimpleNode {
public:
    //Constructor and destructor
    ExpressionNode() = default;
    ~ExpressionNode() override = default;

    //Getters and setters - must be implemented by subclasses
    virtual std::string getDest() const = 0;
    virtual void setDest(std::string dest) = 0;
};