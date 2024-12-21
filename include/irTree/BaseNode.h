#pragma once
#include <string>
#include <sstream>
#include "IrBaseVisitor.h"
#include <set>

//interface for all nodes in the IR tree

class BaseNode {
    public:

        BaseNode();   //default constructor - used for when we want to create derived nodes without initialising text

        BaseNode(std::string text);
        
        virtual ~BaseNode() = default;

        //returns the text of the node
        virtual std::string getText() const;       

        //adds a child to the node, must be implemented to specify the number of children allowed for each node class
        virtual void addChild(BaseNode* child) = 0;

        //print out entire IR tree in text form, including labels and indentation
        //provides a default implementation, but can be overridden by derived classes
        virtual std::string stringifyIRTree() const;    

        virtual std::vector<BaseNode*> getChildren() const;

        virtual std::string accept(IrBaseVisitor* visitor) = 0;     //must be implemented by all nodes

        //takes the ReferencedItems and DefinedItems and returns the variables
        ///NOTE: remove strings and constants using the isVariable function
        virtual std::set<std::string> getReferencedVariables() const = 0;
        virtual std::set<std::string> getDefinedVariables() const = 0;


    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b

        std::vector<BaseNode*> children = {};    //vector of children of the node

        //checks if the item is a variable (temp or user-defined), false if it is a string or positive integer constant
        bool isVariable(std::string item) const;
};