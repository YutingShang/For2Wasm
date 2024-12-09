#pragma once
#include <string>
#include <sstream>

//interface for all nodes in the IR tree

class BaseNode {
    public:
        BaseNode(std::string text);
        
        virtual ~BaseNode() = default;

        virtual std::string getText() const;

        virtual std::vector<std::string> getTextVector() const;

        virtual void addChild(BaseNode* child) = 0;

        virtual std::string stringifyIRTree() const;    //print out IR in text form, including labels and indentation

        virtual std::vector<BaseNode*> getChildren() const;

        virtual bool getVisited() const;

        virtual void setVisited(bool visited);
        
    protected:
        std::vector<std::string> textVector;     //vector of tokens in the instruction, first will contain the operation e.g. MOV a b

        std::vector<BaseNode*> children = {};    //vector of children of the node

        bool visited = false;
};