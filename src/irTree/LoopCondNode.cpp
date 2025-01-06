#include "LoopCondNode.h"

LoopCondNode::LoopCondNode(std::string init, std::string cond, std::string body, std::string step, std::string endloop) {
    this->textVector = {"LOOP", init, cond, body, step, endloop};
}

void LoopCondNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 5) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopCondNode can only have five children");
    }
}

void LoopCondNode::insertChild(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 5 && index < 5) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("LoopCondNode can only have five children");
    }
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

std::string LoopCondNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitLoopCondNode(this);
}

std::string LoopCondNode::getBodyLabel() const {
    return this->textVector[3];
}

std::string LoopCondNode::getEndLoopLabel() const {
    return this->textVector[5];
}

std::set<std::string> LoopCondNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> LoopCondNode::getDefinedVariables() const {
    return {};
}

BaseNode* LoopCondNode::removeCurrentNodeFromIRTree() {
    //first assert that the loop body is empty, if not throw error
    // if (dynamic_cast<EndBlockNode*>(this->children[2]) == nullptr || (dynamic_cast<EndBlockNode*>(this->children[2]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[2])->getText()!="ENDBODY")) {
    //     throw std::runtime_error("LoopCondNode cannot be removed from IR tree, BODY of loop statement is not empty");
    // }

    //get the position of the LOOPCOND node in the parent node
    int indexInParent = this->getPositionInParent();

    BaseNode* parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    this->parent->removeChild(this);          //remove child to make space for the new nodes

    //get what is after the ENDLOOP and attach it to the parent node
    assert(dynamic_cast<EndBlockNode*>(this->children[4]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[4])->getText()=="ENDLOOP");
    EndBlockNode* endLoopNode = dynamic_cast<EndBlockNode*>(this->children[4]);

    BaseNode* child = nullptr;
    if (endLoopNode->getChildren().size() == 1) {        //get the single child of the ENDLOOP node
        child = endLoopNode->getChildren()[0];
        endLoopNode->removeChild(child);   //detach the child from the ENDLOOP node, so it doesn't get deleted
        parent->insertChild(child, indexInParent);
    }
    //otherwise no children to attach to the parent node, just delete the current node
 
    // delete the current node from memory, and recursively any children it is still attached to
    delete this;
 

    //return the child node that has replaced the current node
    return child;
}

