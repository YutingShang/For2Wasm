#include "BaseNode.h"

BaseNode::BaseNode() {}

BaseNode::~BaseNode() {
    for (BaseNode* child : this->children) {
        delete child;
    }
}

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

std::string BaseNode::stringifyIRTree() const
{
    std::string tree = "\t" + getText();

    for (BaseNode *child : this->children)
    {
        tree += "\n" + child->stringifyIRTree();
    }
    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

void BaseNode::setParent(BaseNode* parent) {
    this->parent = parent;
}

BaseNode* BaseNode::getParent() const {
    return this->parent;
}

std::vector<BaseNode *> BaseNode::getChildren() const
{
    return this->children;
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

/////////////////////////ANALYSIS METHODS/////////////////////////

void BaseNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    //default implementation is to do nothing
    //only need to be implemented by nodes that have non-empty referenced var set
}

/////////////////////////TREE MANIPULATION METHODS/////////////////////////

void BaseNode::removeChild(BaseNode* child) {
    child->setParent(nullptr);       //also set the parent of the child to nullptr
    this->children.erase(std::remove(this->children.begin(), this->children.end(), child), this->children.end());
}

void BaseNode::insertSandwichParent(BaseNode* newParent) {
    //first detach everything, but make a copy of the current parent
    BaseNode* oldParent = this->parent;
    int indexInParent = this->getPositionInParent();
    oldParent->removeChild(this);

    //now attach the newParent to the oldParent
    oldParent->addChildAtIndex(newParent, indexInParent);

    //now attach the current node to the newParent (assume just add child to end)
    newParent->addChild(this);
}

/////////////////////////VALIDATION METHODS/////////////////////////
bool BaseNode::isVariable(std::string item) const{

    bool isPositiveInteger = std::all_of(item.begin(), item.end(), ::isdigit);
    
    //it is a variable if it is not a positive integer and is not a string const $
    bool isVariable = !isPositiveInteger && !isStringConstant(item);

    return isVariable;
}

bool BaseNode::isStringConstant(const std::string& operand) const {
    return !operand.empty() && operand[0] == '$';
}