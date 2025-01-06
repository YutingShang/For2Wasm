#pragma once
#include "BaseNode.h"

// SimpleNode is a class for nodes that have a single child node (at most)
class SimpleNode : public BaseNode
{
public:

    SimpleNode();   //default constructor - used for when we want to create derived nodes without initialising text
                    // will call BaseNode default constructor which does nothing
    SimpleNode(std::string text);
    
    ~SimpleNode() override = default;

    void addChild(BaseNode *child) override;
    void insertChild(BaseNode* child, int index) override;

    BaseNode* removeCurrentNodeFromIRTree() override;

    //accept method for visitor pattern must be implemented by child nodes
    //getReferencedVariables and getDefinedVariables must be implemented by child nodes
    //this makes SimpleNode an abstract class for all nodes that have a single child

};