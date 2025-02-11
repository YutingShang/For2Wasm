#pragma once
#include "BaseNode.h"

// SimpleNode is a class for nodes that have a single child node (at most)
class SimpleNode : public BaseNode
{
public:
    //Constructor and destructor
    SimpleNode() = default;   //default constructor - used for when we want to create derived nodes without initialising text - will call BaseNode default constructor which does nothing
    ~SimpleNode() override = default;

    //Getters and setters
    std::shared_ptr<BaseNode> getSingleChild() const;

    //Tree manipulation methods
    void addChild(std::shared_ptr<BaseNode> child) override;
    void addChildAtIndex(std::shared_ptr<BaseNode> child, int index) override;
    std::shared_ptr<BaseNode> removeCurrentNodeFromIRTree() override;

    //inserts a newNode node between the current node and its existing child
    void insertSandwichChild(std::shared_ptr<SimpleNode> newNode);

    //replace the current node with another SimpleNode
    void replaceCurrentNodeInIRTree(std::shared_ptr<SimpleNode> newNode);

    //NOTE: visitor pattern must be implemented by child nodes
    //NOTE: analysis methods must be implemented by child nodes

};