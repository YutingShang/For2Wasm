#pragma once
#include <string>
#include <sstream>

//interface for all nodes in the IR tree

class BaseNode {
    public:
        BaseNode(std::string text);
        
        virtual ~BaseNode() = default;

        virtual std::string getText() const;

        // virtual std::string getTextAt(int index) const;

        virtual void addChild(BaseNode* child) = 0;

        virtual std::string stringifyIRTree() const;

        virtual std::vector<BaseNode*> getChildren() const;

        // virtual void addLabel(std::string label);
        
    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b

        std::vector<BaseNode*> children = {};    //vector of children of the node

        // std::string label = "";                  //label of the node if it has one

        

};