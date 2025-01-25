#include "IfNode.h"

IfNode::IfNode(std::string condition, std::string thenLabel, std::string endLabel) {

    this->textVector = {"IF", condition, thenLabel, endLabel};
}

void IfNode::addChild(BaseNode* child) {
    child->setParent(this);
    if (this->children.size() < 3) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfNode can only have 3 children");
    }
}

void IfNode::addChildAtIndex(BaseNode* child, int index) {
    child->setParent(this);
    if (this->children.size() < 3 && index < 3) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("IfNode can only have 3 children");
    }
}

std::string IfNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();

    for (int i=1; i<this->textVector.size(); i++){
        tree += "\n" + this->textVector[i] +": " + this->children[i-1]->stringifyIRTree();
    }

    return tree;
}

std::string IfNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitIfNode(this);
}

std::set<std::string> IfNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> IfNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> IfNode::getReferencedExpressions() const {
    return {};
}

BaseNode* IfNode::removeCurrentNodeFromIRTree() {
    //first assert that the then statement is empty, if not throw error
    if (dynamic_cast<EndBlockNode*>(this->children[1]) == nullptr || (dynamic_cast<EndBlockNode*>(this->children[1]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[1])->getText()!="ENDTHEN")) {
        throw std::runtime_error("IfNode cannot be removed from IR tree, body of THEN statement is not empty");
    }

    //get the position of the IF node in the parent node
    int indexInParent = this->getPositionInParent();

    BaseNode* parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    this->parent->removeChild(this);          //remove child to make space for the new nodes

    //get what is after the ENDIF and attach it to the parent node
    assert(dynamic_cast<EndBlockNode*>(this->children[2]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[2])->getText()=="ENDIF");
    EndBlockNode* endIfNode = dynamic_cast<EndBlockNode*>(this->children[2]);

    BaseNode* child = nullptr;
    if (endIfNode->getChildren().size() == 1) {        //get the single child of the ENDIF node
        child = endIfNode->getChildren()[0];
        endIfNode->removeChild(child);    //remove the child from the ENDIF node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    } 
    //otherwise no children to attach to the parent node, just delete the current node

    // delete the current node from memory
    delete this;

    //return the child node that has replaced the current node
    return child;
}
