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
        
        virtual ~BaseNode();

        //returns the text of the node
        virtual std::string getText() const;   

        //print out entire IR tree in text form, including labels and indentation
        //provides a default implementation, but can be overridden by derived classes
        virtual std::string stringifyIRTree() const;     

        //adds a child to the node (either at the end or at a specific index), must be implemented to specify the number of children allowed for each node class
        //different to insertSandwichChild defined in SimpleNode
        ///NOTE: addChild and addChildAtIndex should also setParent of the child to this node
        virtual void addChild(BaseNode* child) = 0;
        virtual void addChildAtIndex(BaseNode* child, int index) = 0;
        virtual void removeChild(BaseNode* child);
        virtual std::vector<BaseNode*> getChildren() const;

        virtual void setParent(BaseNode* parent);
        virtual BaseNode* getParent() const;
        virtual void insertSandwichParent(BaseNode* newParent);   //inserts the newParent node between the current parent and the current node


        //takes the ReferencedItems and DefinedItems and returns the variables
        ///NOTE: remove strings and constants using the isVariable function
        virtual std::set<std::string> getReferencedVariables() const = 0;
        virtual std::set<std::string> getDefinedVariables() const = 0;
        virtual std::set<std::string> getReferencedExpressions() const = 0;     //these are the expressions that are used/referenced in an instruction
                                                                                //Different to generated expressions which guarantee expression not immediately killed as well

        //visitor pattern - must be implemented by all nodes
        virtual std::string accept(IrBaseVisitor* visitor) = 0;     

        virtual int getPositionInParent() const;

        //edits IR tree by removing the node, and adding its children to the parent node (calls getPositionInParent to find the correct position)
        ///NOTE: must be implemented by simple node, and then how to remove IF and LOOP nodes etc.
        ///RETURNS: this will return the child node that has replaced the current node - could be nullptr if the node has no children!!
        ///CAREFUL: the caller of this function must not use the current node after it has been removed from the IR tree to avoid use-after-free errors
        virtual BaseNode* removeCurrentNodeFromIRTree() = 0;

        //used in copy propagation to replace a referenced variable with a new variable
        //a substitution [y/x] means all instances of referenced variable x are replaced with y
        // e.g. x+x -> y+y, where referenced var set only contained {x}
        // only need to be implemented by nodes that have non-empty referenced var set
        // default implementation is to do nothing
        virtual void replaceReferencedVariable(std::string oldVar, std::string newVar);

        //just copy this node, without its children or parent (not a deep copy)
        //return type is BaseNode* but the underlying dynamic type is the derived class
        virtual BaseNode* cloneContent() const = 0;



    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b

        BaseNode* parent = nullptr;
        std::vector<BaseNode*> children = {};    //vector of children of the node

        //checks if the item is a variable (temp or user-defined), false if it is a string or positive integer constant
        bool isVariable(std::string item) const;

        
};