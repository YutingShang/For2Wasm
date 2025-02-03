#include "LoopCondNode.h"

LoopCondNode::LoopCondNode(std::string labelNumber): labelNumber(labelNumber) {
    this->textVector = {"LOOP", "init" + labelNumber, "cond" + labelNumber, "body" + labelNumber, "step" + labelNumber, "endloop" + labelNumber};
}

std::shared_ptr<BaseNode> LoopCondNode::cloneContent() const {
    return std::make_shared<LoopCondNode>(getLabelNumber());
}

std::string LoopCondNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();
    tree += "\n" + this->textVector[1] +": " + this->children[0]->stringifyIRTree();      //adds init label
    tree += "\n" + this->textVector[2] +": " + this->children[1]->stringifyIRTree();      //adds cond label
    tree += "\n" + this->textVector[3] +": " + this->children[2]->stringifyIRTree();      //adds body label
    tree += "\n" + this->textVector[4] +": " + this->children[3]->stringifyIRTree();      //adds step label
    tree += "\n" + this->textVector[5] +": " + this->children[4]->stringifyIRTree();      //adds endloop label
    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LoopCondNode::getInitLabel() const {
    return this->textVector[1];
}

std::string LoopCondNode::getCondLabel() const {
    return this->textVector[2];
}

std::string LoopCondNode::getBodyLabel() const {
    return this->textVector[3];
}

std::string LoopCondNode::getStepLabel() const {
    return this->textVector[4];
}

std::string LoopCondNode::getEndLoopLabel() const {
    return this->textVector[5];
}

std::string LoopCondNode::getLabelNumber() const {
    return this->labelNumber;
}

std::shared_ptr<BaseNode> LoopCondNode::getInitNode() const {
    return this->children[0];
}

std::shared_ptr<BaseNode> LoopCondNode::getCondNode() const {
    return this->children[1];
}

std::shared_ptr<BaseNode> LoopCondNode::getBodyNode() const {
    return this->children[2];
}

std::shared_ptr<BaseNode> LoopCondNode::getStepNode() const {
    return this->children[3];
}

std::shared_ptr<BaseNode> LoopCondNode::getEndLoopNode() const {
    return this->children[4];
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LoopCondNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitLoopCondNode(std::static_pointer_cast<LoopCondNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> LoopCondNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> LoopCondNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> LoopCondNode::getReferencedExpressions() const {
    return {};
}

/////////////////////////TREE MANIPULATION/////////////////////////

void LoopCondNode::addChild(std::shared_ptr<BaseNode> child) {
    child->setParent(shared_from_this());

    if (this->children.size() < 5) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopCondNode can only have five children");
    }
}

void LoopCondNode::addChildAtIndex(std::shared_ptr<BaseNode> child, int index) {
    child->setParent(shared_from_this());
    if (this->children.size() < 5 && index < 5) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("LoopCondNode can only have five children");
    }
}

std::shared_ptr<BaseNode> LoopCondNode::removeCurrentNodeFromIRTree() {
    //first assert that the loop body is empty, if not throw error
    std::shared_ptr<EndBlockNode> bodyNode = std::dynamic_pointer_cast<EndBlockNode>(this->getBodyNode());
    if (bodyNode == nullptr || (bodyNode != nullptr && bodyNode->getText()!="ENDBODY")) {
        throw std::runtime_error("LoopCondNode cannot be removed from IR tree, BODY of loop statement is not empty");
    }

    //get the position of the LOOPCOND node in the parent node
    int indexInParent = this->getPositionInParent();

    std::shared_ptr<BaseNode> parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    this->parent.lock()->removeChild(*this);          //remove child to make space for the new nodes

    //get what is after the ENDLOOP and attach it to the parent node
    std::shared_ptr<EndBlockNode> endLoopNode = std::dynamic_pointer_cast<EndBlockNode>(this->getEndLoopNode());
    assert(endLoopNode != nullptr && endLoopNode->getText()=="ENDLOOP");

    std::shared_ptr<BaseNode> child = nullptr;
    if (endLoopNode->getChildren().size() == 1) {        //get the single child of the ENDLOOP node
        child = endLoopNode->getSingleChild();
        endLoopNode->removeChild(*child);   //detach the child from the ENDLOOP node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    }
    //otherwise no children to attach to the parent node, just delete the current node (or handled by shared_ptr)

    //return the child node that has replaced the current node
    return child;
}

