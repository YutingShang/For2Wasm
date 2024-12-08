#pragma once
#include "BaseNode.h"

class SimpleNode : public BaseNode
{
public:
    SimpleNode(std::string text);
    
    ~SimpleNode() override = default;

    void addChild(BaseNode *child) override;

};