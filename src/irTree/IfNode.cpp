#include "IfNode.h"
#include "IfElseNode.h"

IfNode::IfNode(std::string labelNumber): labelNumber(labelNumber) {
    conditionLabel = "cond" + labelNumber;
    thenLabel = "then" + labelNumber;
    endLabel = "endif" + labelNumber;
}

std::shared_ptr<BaseNode> IfNode::cloneContent() const {
    return std::make_shared<IfNode>(labelNumber);
}

std::string IfNode::getText() const {
    return "IF " + conditionLabel + " " + thenLabel + " " + endLabel;
}

std::string IfNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();

    tree += "\n" + conditionLabel + ": " + getConditionNode()->stringifyIRTree();
    tree += "\n" + thenLabel + ": " + getThenNode()->stringifyIRTree();
    tree += "\n" + endLabel + ": " + getEndIfNode()->stringifyIRTree();

    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string IfNode::getConditionLabel() const {
    return conditionLabel;
}

std::string IfNode::getThenLabel() const {
    return thenLabel;
}

std::string IfNode::getEndLabel() const {
    return endLabel;
}

std::string IfNode::getLabelNumber() const {
    return labelNumber;
}

std::shared_ptr<BaseNode> IfNode::getConditionNode() const {
    return children[0];
}

std::shared_ptr<BaseNode> IfNode::getThenNode() const {
    return children[1];
}

std::shared_ptr<BaseNode> IfNode::getEndIfNode() const {
    return children[2];
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string IfNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitIfNode(std::static_pointer_cast<IfNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> IfNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> IfNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> IfNode::getReferencedExpressions() const {
    return {};
}


/////////////////////////TREE MANIPULATION METHODS/////////////////////////

void IfNode::addChild(std::shared_ptr<BaseNode> child) {
    child->setParent(shared_from_this());
    if (this->children.size() < 3) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfNode can only have 3 children");
    }
}

void IfNode::addChildAtIndex(std::shared_ptr<BaseNode> child, int index) {
    child->setParent(shared_from_this());
    if (this->children.size() < 3 && index < 3) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("IfNode can only have 3 children");
    }
}       

std::shared_ptr<BaseNode> IfNode::removeCurrentNodeFromIRTree() {
    //first assert that the then statement is empty, if not throw error
    std::shared_ptr<EndBlockNode> thenNode = std::dynamic_pointer_cast<EndBlockNode>(this->getThenNode());
    if (thenNode == nullptr || (thenNode != nullptr && thenNode->getText()!="ENDTHEN")) {
        throw std::runtime_error("IfNode cannot be removed from IR tree, body of THEN statement is not empty");
    }

    //get the position of the IF node in the parent node
    int indexInParent = this->getPositionInParent();

    std::shared_ptr<BaseNode> parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    parent->removeChild(*this);          //remove child to make space for the new nodes

    //get what is after the ENDIF and attach it to the parent node
    std::shared_ptr<EndBlockNode> endIfNode = std::dynamic_pointer_cast<EndBlockNode>(this->getEndIfNode());
    assert(endIfNode != nullptr && endIfNode->getText()=="ENDIF");

    std::shared_ptr<BaseNode> child = nullptr;
    if (endIfNode->getChildren().size() == 1) {        //get the single child of the ENDIF node
        child = endIfNode->getChildren()[0];
        endIfNode->removeChild(*child);    //remove the child from the ENDIF node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    } 
    //otherwise no children to attach to the parent node

    // should have no references to the current node in the IR tree, handled by shared_ptr

    //return the child node that has replaced the current node
    return child;
}

std::shared_ptr<IfElseNode> IfNode::convertToIfElseNode() {

    //create a new IfElseNode with the same condition, thenLabel, and endLabel
    std::shared_ptr<IfElseNode> ifElseNode = std::make_shared<IfElseNode>(labelNumber);
    
    std::shared_ptr<BaseNode> conditionChild = this->getConditionNode();
    std::shared_ptr<BaseNode> thenChild = this->getThenNode();
    std::shared_ptr<BaseNode> endIfChild = this->getEndIfNode();

    this->removeChild(*conditionChild);
    this->removeChild(*thenChild);
    this->removeChild(*endIfChild);
    
    //then we move the children of the current node to the new node
    ifElseNode->addChild(conditionChild);    //condition
    ifElseNode->addChild(thenChild);    //then block

    // create a new ELSE block
    std::shared_ptr<EndBlockNode> endElseNode = std::make_shared<EndBlockNode>("ENDELSE");
    ifElseNode->addChild(endElseNode);          //else block
    ifElseNode->addChild(endIfChild);    //endif block

    //get the current node's parent and add the new node to the parent
    std::shared_ptr<BaseNode> parent = this->getParent();
    parent->removeChild(*this);
    parent->addChild(ifElseNode);
    
    return ifElseNode;
}