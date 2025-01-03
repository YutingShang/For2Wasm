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
    // trim the last whitespace
    text = text.substr(0, text.size() - 1);
    return text;
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

//checks if the item is a variable (temp or user-defined), false if it is a string or positive integer constant
bool BaseNode::isVariable(std::string item) const{

    bool isPositiveInteger = true;
    for (char c : item) {
        if (!isdigit(c)) {
            isPositiveInteger = false;
            break;
        }
    }
    //it is a variable if it is not a positive integer and is not a string const $
    bool isVariable = !isPositiveInteger && item[0] != '$';

    return isVariable;
}

void BaseNode::setParent(BaseNode* parent) {
    this->parent = parent;
}

BaseNode* BaseNode::getParent() const {
    return this->parent;
}

void BaseNode::removeChild(BaseNode* child) {
    this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());
}

int BaseNode::getPositionInParent() const {
    if (this->parent == nullptr) {
        throw std::runtime_error("Parent node is null, cannot get position in parent");
    }
    std::vector<BaseNode*> parentChildren = this->parent->getChildren();

    //get the index of the current instruction in the parent node as an iterator
    auto childrenIterator = std::find(parentChildren.begin(), parentChildren.end(), this);
    int indexInParent = std::distance(parentChildren.begin(), childrenIterator);

    return indexInParent;
}

void BaseNode::removeCurrentNodeFromIRTree(int indexInParent) {
    //remove the node from the parent node
    this->parent->removeChild(this);

    //add the children to the parent node
    int insertIndex = indexInParent;
    for (BaseNode* child : this->children) {
        child->setParent(this->parent);
        this->parent->insertChild(child, insertIndex);
        insertIndex++;
    }

    // delete the current node from memory
    delete this;
}