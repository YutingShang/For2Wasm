#include "LoopNode.h"

LoopNode::LoopNode(std::string labelNumber): labelNumber(labelNumber) {
    this->bodyLabel = "body" + labelNumber;
    this->endLoopLabel = "endloop" + labelNumber;
}

std::shared_ptr<BaseNode> LoopNode::cloneContent() const {
    return std::make_shared<LoopNode>(getLabelNumber());
}

std::string LoopNode::getText() const {
    return "LOOP " + bodyLabel + " " + endLoopLabel;
}

std::string LoopNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();
    tree += "\n" + this->bodyLabel +": " + this->children[0]->stringifyIRTree();      //adds body label
    tree += "\n" + this->endLoopLabel +": " + this->children[1]->stringifyIRTree();      //adds endloop label
    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LoopNode::getBodyLabel() const {
    return this->bodyLabel;
}

std::string LoopNode::getEndloopLabel() const {
    return this->endLoopLabel;
}

std::string LoopNode::getLabelNumber() const {
    return this->labelNumber;
}

std::shared_ptr<BaseNode> LoopNode::getBodyNode() const {
    return this->children[0];
}

std::shared_ptr<BaseNode> LoopNode::getEndLoopNode() const {
    return this->children[1];
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LoopNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitLoopNode(std::static_pointer_cast<LoopNode>(shared_from_this()));
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

void LoopNode::addChild(std::shared_ptr<BaseNode> child) {
    child->setParent(shared_from_this());

    if (this->children.size() < 2) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("LoopNode can only have two children");
    }
}

void LoopNode::addChildAtIndex(std::shared_ptr<BaseNode> child, int index) {
    child->setParent(shared_from_this());
    if (this->children.size() < 2 && index < 2) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("LoopNode can only have two children");
    }
}

std::shared_ptr<BaseNode> LoopNode::removeCurrentNodeFromIRTree() {

    //first assert that the loop body is empty, if not throw error
    std::shared_ptr<EndBlockNode> bodyNode = std::dynamic_pointer_cast<EndBlockNode>(this->getBodyNode());
    if (bodyNode == nullptr || (bodyNode != nullptr && bodyNode->getText()!="ENDBODY")) {
        throw std::runtime_error("LoopNode cannot be removed from IR tree, BODY of loop statement is not empty");
    }

    //get the position of the LOOP node in the parent node
    int indexInParent = this->getPositionInParent();

    std::shared_ptr<BaseNode> parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    parent->removeChild(*this);          //remove child to make space for the new nodes

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

void LoopNode::insertSandwichBodyChild(std::shared_ptr<SimpleNode> newNode) {
    //remove the current body child, and attach it to the newNode
    assert(this->children.size() == 2);

    std::shared_ptr<BaseNode> currentBodyStartNode = this->getBodyNode();
    this->removeChild(*currentBodyStartNode);
    newNode->addChild(currentBodyStartNode);

    //attach the newNode to the LoopNode as the body child
    this->addChildAtIndex(newNode, 0);

}