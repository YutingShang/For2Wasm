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
    void addChildAtIndex(BaseNode* child, int index) override;

    //from BaseNode
    BaseNode* removeCurrentNodeFromIRTree() override;

    //inserts a newNode node between the current node and its existing child
    void insertSandwichChild(SimpleNode* newNode);

    //replace the current node with another SimpleNode
    void replaceCurrentNodeInIRTree(SimpleNode* newNode);

    //accept method for visitor pattern must be implemented by child nodes
    //getReferencedVariables and getDefinedVariables and getGeneratedExpressions must be implemented by child nodes
    //this makes SimpleNode an abstract class for all nodes that have a single child

};