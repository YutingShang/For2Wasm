#pragma once
#include <string>
#include <sstream>
#include "IrBaseVisitor.h"

//interface for all nodes in the IR tree

class BaseNode {
    public:

        BaseNode();   //default constructor - used for when we want to create derived nodes without initialising text

        BaseNode(std::string text);
        
        virtual ~BaseNode() = default;

        virtual std::string getText() const;       //get the text of the node

        virtual std::vector<std::string> getTextVector() const;

        virtual void addChild(BaseNode* child) = 0;

        virtual std::string stringifyIRTree() const;    //print out entire IR tree in text form, including labels and indentation

        virtual std::vector<BaseNode*> getChildren() const;

        virtual bool getVisited() const;

        virtual void setVisited(bool visited);

        virtual std::string accept(IrBaseVisitor* visitor) = 0;     //must be implemented by all nodes
        
    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b

        std::vector<BaseNode*> children = {};    //vector of children of the node

        bool visited = false;
};