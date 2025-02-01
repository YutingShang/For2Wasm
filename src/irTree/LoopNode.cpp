#include "LoopNode.h"

LoopNode::LoopNode(std::string labelNumber): labelNumber(labelNumber) {
    this->textVector = {"LOOP", "body" + labelNumber, "endloop" + labelNumber};
}

BaseNode* LoopNode::cloneContent() const {
    return new LoopNode(getLabelNumber());
}

std::string LoopNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();
    tree += "\n" + this->textVector[1] +": " + this->children[0]->stringifyIRTree();      //adds body label
    tree += "\n" + this->textVector[2] +": " + this->children[1]->stringifyIRTree();      //adds endloop label
    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LoopNode::getBodyLabel() const {
    return this->textVector[1];
}

std::string LoopNode::getEndloopLabel() const {
    return this->textVector[2];
}

std::string LoopNode::getLabelNumber() const {
    return this->labelNumber;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LoopNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLoopNode(this);
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> LoopNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> LoopNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> LoopNode::getReferencedExpressions() const {
    return {};
}

/////////////////////////TREE MANIPULATION/////////////////////////

void LoopNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 2) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopNode can only have two children");
    }
}

void LoopNode::addChildAtIndex(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 2 && index < 2) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("LoopNode can only have two children");
    }
}

BaseNode* LoopNode::removeCurrentNodeFromIRTree() {

    //first assert that the loop body is empty, if not throw error
    if (dynamic_cast<EndBlockNode*>(this->children[0]) == nullptr || (dynamic_cast<EndBlockNode*>(this->children[0]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[0])->getText()!="ENDBODY")) {
        throw std::runtime_error("LoopNode cannot be removed from IR tree, BODY of loop statement is not empty");
    }

    //get the position of the LOOP node in the parent node
    int indexInParent = this->getPositionInParent();

    BaseNode* parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    this->parent->removeChild(this);          //remove child to make space for the new nodes

    //get what is after the ENDLOOP and attach it to the parent node
    assert(dynamic_cast<EndBlockNode*>(this->children[1]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[1])->getText()=="ENDLOOP");
    EndBlockNode* endLoopNode = dynamic_cast<EndBlockNode*>(this->children[1]);

    BaseNode* child = nullptr;
    if (endLoopNode->getChildren().size() == 1) {        //get the single child of the ENDLOOP node
        child = endLoopNode->getChildren()[0];
        endLoopNode->removeChild(child);   //detach the child from the ENDLOOP node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    } 
    //otherwise no children to attach to the parent node, just delete the current node

    // delete the current node from memory
    // delete this;

    //return the child node that has replaced the current node
    return child;
}

void LoopNode::insertSandwichBodyChild(SimpleNode* newNode) {
    //remove the current body child, and attach it to the newNode
    assert(this->children.size() == 2);

    BaseNode* currentBodyStartNode = this->children[0];
    this->removeChild(currentBodyStartNode);
    newNode->addChild(currentBodyStartNode);

    //attach the newNode to the LoopNode
    this->addChild(newNode);

}