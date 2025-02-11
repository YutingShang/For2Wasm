#include "BaseNode.h"

std::string BaseNode::stringifyIRTree() const
{
    std::string tree = "\t" + getText();

    for (const std::shared_ptr<BaseNode> child : this->children)
    {
        tree += "\n" + child->stringifyIRTree();
    }
    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

void BaseNode::setParent(std::shared_ptr<BaseNode> parent) {
    this->parent = parent;      //set to weak pointer
}

std::shared_ptr<BaseNode> BaseNode::getParent() const {
    if (this->parent.expired()) {
        throw std::runtime_error("Parent node has expired, cannot get parent");
    }
    return this->parent.lock();
}

std::vector<std::shared_ptr<BaseNode>> BaseNode::getChildren() const
{
    return this->children;
}

int BaseNode::getPositionInParent() const {
    if (this->parent.expired()) {
        throw std::runtime_error("Parent node has expired, cannot get position in parent");
    }
    std::vector<std::shared_ptr<BaseNode>> parentChildren = this->parent.lock()->getChildren();

    //get the index of the current instruction in the parent node as an iterator
    auto childrenIterator = std::find_if(parentChildren.begin(), parentChildren.end(), [this](const std::shared_ptr<BaseNode>& vectorElt){return vectorElt.get() == this;});
    int indexInParent = std::distance(parentChildren.begin(), childrenIterator);

    return indexInParent;
}

/////////////////////////ANALYSIS METHODS/////////////////////////

void BaseNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    //default implementation is to do nothing
    //only need to be implemented by nodes that have non-empty referenced var set
}

/////////////////////////TREE MANIPULATION METHODS/////////////////////////

void BaseNode::removeChild(BaseNode& child) {
    child.setParent(nullptr);       //also set the parent of the child to nullptr
    //uses a lambda to compare the child (to remove) with the elements in the vector
    //uses erase-remove idiom, first moves the item to the end of the vector, then erases it
    this->children.erase(std::remove_if(this->children.begin(), this->children.end(), [&child](const std::shared_ptr<BaseNode>& vectorElt){return vectorElt.get() == &child;}), this->children.end());
}

void BaseNode::insertSandwichParent(std::shared_ptr<BaseNode> newParent) {
    //first detach everything, but make a copy of the current parent
    std::shared_ptr<BaseNode> oldParent = this->parent.lock();
    int indexInParent = this->getPositionInParent();
    oldParent->removeChild(*this);

    //now attach the newParent to the oldParent
    oldParent->addChildAtIndex(newParent, indexInParent);

    //now attach the current node to the newParent (assume just add child to end)
    newParent->addChild(shared_from_this());
}

