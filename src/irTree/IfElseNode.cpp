#include "IfElseNode.h"

IfElseNode::IfElseNode(std::string labelNumber): labelNumber(labelNumber) {
    this->textVector = {"IF", "cond" + labelNumber, 
                                "then" + labelNumber, 
                                "else" + labelNumber, 
                                "endif" + labelNumber};
}

std::shared_ptr<BaseNode> IfElseNode::cloneContent() const {
    return std::make_shared<IfElseNode>(labelNumber);
}

std::string IfElseNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();       //IF cond then else end

    for (int i=1; i<this->textVector.size(); i++){        //prints the label for cond, then, else, end
        tree += "\n" + this->textVector[i] +": " + this->children[i-1]->stringifyIRTree();
    }

    return tree;
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string IfElseNode::getConditionLabel() const {
    return textVector[1];
}

std::string IfElseNode::getThenLabel() const {
    return textVector[2];
}

std::string IfElseNode::getElseLabel() const {
    return textVector[3];
}

std::string IfElseNode::getEndLabel() const {
    return textVector[4];
}

std::string IfElseNode::getLabelNumber() const {
    return labelNumber;
}

std::shared_ptr<BaseNode> IfElseNode::getConditionNode() const {
    return children[0];
}

std::shared_ptr<BaseNode> IfElseNode::getThenNode() const {
    return children[1];
}

std::shared_ptr<BaseNode> IfElseNode::getElseNode() const {
    return children[2];
}

std::shared_ptr<BaseNode> IfElseNode::getEndIfNode() const {
    return children[3];
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string IfElseNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitIfElseNode(std::static_pointer_cast<IfElseNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> IfElseNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> IfElseNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> IfElseNode::getReferencedExpressions() const {
    return {};
}

/////////////////////////TREE MANIPULATION METHODS/////////////////////////
void IfElseNode::addChild(std::shared_ptr<BaseNode> child) {
    child->setParent(shared_from_this());

    if (this->children.size() < 4) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfElseNode can only have four children");
    }
}

void IfElseNode::addChildAtIndex(std::shared_ptr<BaseNode> child, int index) {
    child->setParent(shared_from_this());

    //will just append to the end if vector is of size 3, and inserting to index 3
    if (this->children.size() < 4 && index < 4) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("IfElseNode can only have four children");
    }
}


std::shared_ptr<BaseNode> IfElseNode::removeCurrentNodeFromIRTree() {
    //first assert that the then and else statements are empty, if not throw error
    std::shared_ptr<EndBlockNode> thenNode = std::dynamic_pointer_cast<EndBlockNode>(this->getThenNode());
    if (thenNode == nullptr || (thenNode != nullptr && thenNode->getText()!="ENDTHEN")) {
        throw std::runtime_error("IfNode cannot be removed from IR tree, body of THEN statement is not empty");
    }
    std::shared_ptr<EndBlockNode> elseNode = std::dynamic_pointer_cast<EndBlockNode>(this->getElseNode());
    if (elseNode == nullptr || (elseNode != nullptr && elseNode->getText()!="ENDELSE")) {
        throw std::runtime_error("IfNode cannot be removed from IR tree, body of ELSE statement is not empty");
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
        child = endIfNode->getSingleChild();
        endIfNode->removeChild(*child);           //remove the child from the ENDIF node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    } 
    //otherwise no children to attach to the parent node

    //should have no more references to the current node (at least none in the IR tree), will be handled by smart pointers

    //return the child node that has replaced the current node
    return child;
}