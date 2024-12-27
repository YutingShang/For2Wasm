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

        //print out entire IR tree in text form, including labels and indentation
        //provides a default implementation, but can be overridden by derived classes
        virtual std::string stringifyIRTree() const;     

        //adds a child to the node, must be implemented to specify the number of children allowed for each node class
        ///NOTE: addChild and insertChild should also setParent of the child to this node
        virtual void addChild(BaseNode* child) = 0;
        virtual void insertChild(BaseNode* child, int index) = 0;
        virtual void removeChild(BaseNode* child);
        virtual std::vector<BaseNode*> getChildren() const;

        virtual void setParent(BaseNode* parent);
        virtual BaseNode* getParent() const;

        //takes the ReferencedItems and DefinedItems and returns the variables
        ///NOTE: remove strings and constants using the isVariable function
        virtual std::set<std::string> getReferencedVariables() const = 0;
        virtual std::set<std::string> getDefinedVariables() const = 0;

        //visitor pattern - must be implemented by all nodes
        virtual std::string accept(IrBaseVisitor* visitor) = 0;     

        virtual int getPositionInParent() const;

        //edits IR tree by removing the node, and adding its children to the parent node
        //takes in the index of the current node in the parent node, so its children can be inserted in the correct position
        virtual void removeCurrentNodeFromIRTree(int indexInParent);




    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b

        BaseNode* parent = nullptr;
        std::vector<BaseNode*> children = {};    //vector of children of the node

        //checks if the item is a variable (temp or user-defined), false if it is a string or positive integer constant
        bool isVariable(std::string item) const;

        
};