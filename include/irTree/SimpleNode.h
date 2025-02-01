#pragma once
#include "BaseNode.h"

// SimpleNode is a class for nodes that have a single child node (at most)
class SimpleNode : public BaseNode
{
public:
    //Constructor and destructor
    SimpleNode();   //default constructor - used for when we want to create derived nodes without initialising text - will call BaseNode default constructor which does nothing
    SimpleNode(std::string text);
    ~SimpleNode() override = default;

    //Tree manipulation methods
    void addChild(BaseNode *child) override;
    void addChildAtIndex(BaseNode* child, int index) override;
    BaseNode* removeCurrentNodeFromIRTree() override;

    //inserts a newNode node between the current node and its existing child
    void insertSandwichChild(SimpleNode* newNode);

    //replace the current node with another SimpleNode
    void replaceCurrentNodeInIRTree(SimpleNode* newNode);

    //NOTE: visitor pattern must be implemented by child nodes
    //NOTE: analysis methods must be implemented by child nodes

};