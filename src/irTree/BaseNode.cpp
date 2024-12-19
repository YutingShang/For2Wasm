#include "BaseNode.h"

BaseNode::BaseNode() {}

BaseNode::BaseNode(std::string text)
{

    std::istringstream stream(text);
    std::string token;

    while (stream >> token)
    { // split the line into tokens, removing whitespaces
        this->textVector.push_back(token);
    }
}

std::string BaseNode::getText() const
{
    std::string text = "";
    for (std::string token : this->textVector)
    {
        text += token + " ";
    }
    //trim the last whitespace
    text = text.substr(0, text.size() - 1);
    return text;
}

std::vector<std::string> BaseNode::getTextVector() const
{
    return this->textVector;
}

std::vector<BaseNode *> BaseNode::getChildren() const
{
    return this->children;
}

std::string BaseNode::stringifyIRTree() const
{
    std::string tree = "\t" + getText();

    for (BaseNode *child : this->children)
    {
        tree += "\n" + child->stringifyIRTree();
    }
    return tree;
}

bool BaseNode::getVisited() const {
    return this->visited;
}

void BaseNode::setVisited(bool visited) {
    this->visited = visited;
}