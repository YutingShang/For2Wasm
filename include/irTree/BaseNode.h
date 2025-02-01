#pragma once
#include <string>
#include <sstream>
#include "IrBaseVisitor.h"
#include <set>

//interface for all nodes in the IR tree

class BaseNode {
    public:
        //Constructor and destructor
        BaseNode();   //default constructor - used for when we want to create derived nodes without initialising text
        BaseNode(std::string text);
        virtual ~BaseNode();       //destructor - recursively deletes all children
        virtual BaseNode* cloneContent() const = 0;     //just copy this node, without its children or parent (not a deep copy). Must be implemented by derived classes to give the correct dynamic type
        virtual std::string getText() const;    //returns the text of the node
        virtual std::string stringifyIRTree() const;      //print out entire IR tree in text form, including labels and indentation - can be overridden by derived classes

        //Getters and setters
        virtual void setParent(BaseNode* parent);
        virtual BaseNode* getParent() const;
        virtual std::vector<BaseNode*> getChildren() const;
        virtual int getPositionInParent() const;

        //Visitor pattern - must be implemented by all nodes
        virtual std::string accept(IrBaseVisitor* visitor) = 0;     

        //Analysis methods
        //Takes the ReferencedItems and DefinedItems and returns the variables
        //NOTE: remove strings and constants using the isVariable function
        virtual std::set<std::string> getReferencedVariables() const = 0;
        virtual std::set<std::string> getDefinedVariables() const = 0;
        virtual std::set<std::string> getReferencedExpressions() const = 0;     //these are the expressions that are used/referenced in an instruction - Different to generated expressions which guarantee expression not immediately killed as well
        //used in copy propagation to replace a referenced variable with a new variable
        //a substitution [y/x] means all instances of referenced variable x are replaced with y
        // e.g. x+x -> y+y, where referenced var set only contained {x}
        //NOTE: only need to be implemented by nodes that have non-empty referenced var set
        //default implementation is to do nothing
        virtual void replaceReferencedVariable(std::string oldVar, std::string newVar);

        //Tree manipulation methods
        //Adds a NEW child to the node (either at the end or at a specific index), must be implemented to specify the number of children allowed for each node class - Different to insertSandwichChild defined in SimpleNode
        //NOTE: must handle the parent-child relationship! (e.g. use setParent in addChild and addChildAtIndex)
        virtual void addChild(BaseNode* child) = 0;
        virtual void addChildAtIndex(BaseNode* child, int index) = 0;
        virtual void removeChild(BaseNode* child);
        virtual void insertSandwichParent(BaseNode* newParent);   //inserts the newParent node between the current parent and the current node

        //edits IR tree by removing the node, and adding its children to the parent node (calls getPositionInParent to find the correct position)
        //NOTE: must be implemented by simple node, and then how to remove IF and LOOP nodes etc.
        //RETURNS: this will return the child node that has replaced the current node - could be nullptr if the node has no children!!
        //CAREFUL: the caller of this function must not use the current node after it has been removed from the IR tree to avoid use-after-free errors
        virtual BaseNode* removeCurrentNodeFromIRTree() = 0;

    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b
        BaseNode* parent = nullptr;
        std::vector<BaseNode*> children = {};    //vector of children of the node

        //VALIDATION METHODS
        //checks if the item is a variable (temp or user-defined), false if it is a string or positive integer constant
        bool isVariable(std::string item) const;
        //checks if the operand is a valid operand for instructions that take in variables or integers
        bool isStringConstant(const std::string& item) const;

};