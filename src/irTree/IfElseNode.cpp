#include "IfElseNode.h"

IfElseNode::IfElseNode(std::string condition, std::string thenLabel, std::string elseLabel, std::string endLabel) {

    this->textVector = {"IF", condition, thenLabel, elseLabel, endLabel};
}

BaseNode* IfElseNode::cloneContent() const {
    return new IfElseNode(textVector[1], textVector[2], textVector[3], textVector[4]);
}

void IfElseNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 4) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfElseNode can only have four children");
    }
}

void IfElseNode::addChildAtIndex(BaseNode* child, int index) {
    child->setParent(this);

    //will just append to the end if vector is of size 3, and inserting to index 3
    if (this->children.size() < 4 && index < 4) {
        this->children.insert(this->children.begin() + index, child);
    } else {
        throw std::runtime_error("IfElseNode can only have four children");
    }
}

std::string IfElseNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();       //IF cond then else end

    for (int i=1; i<this->textVector.size(); i++){        //prints the label for cond, then, else, end
        tree += "\n" + this->textVector[i] +": " + this->children[i-1]->stringifyIRTree();
    }

    return tree;
}

std::string IfElseNode::accept(IrBaseVisitor* visitor) {
    return visitor->visitIfElseNode(this);
}

std::set<std::string> IfElseNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> IfElseNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> IfElseNode::getReferencedExpressions() const {
    return {};
}

BaseNode* IfElseNode::removeCurrentNodeFromIRTree() {
    //first assert that the then and else statements are empty, if not throw error
    if (dynamic_cast<EndBlockNode*>(this->children[1]) == nullptr || (dynamic_cast<EndBlockNode*>(this->children[1]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[1])->getText()!="ENDTHEN")) {
        throw std::runtime_error("IfNode cannot be removed from IR tree, body of THEN statement is not empty");
    }
    if (dynamic_cast<EndBlockNode*>(this->children[2]) == nullptr || (dynamic_cast<EndBlockNode*>(this->children[2]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[2])->getText()!="ENDELSE")) {
        throw std::runtime_error("IfNode cannot be removed from IR tree, body of ELSE statement is not empty");
    }

    //get the position of the IF node in the parent node
    int indexInParent = this->getPositionInParent();

    BaseNode* parent = this->getParent();     //keep a copy of the parent node, removeChild will set the parent to nullptr
    this->parent->removeChild(this);          //remove child to make space for the new nodes

    //get what is after the ENDIF and attach it to the parent node
    assert(dynamic_cast<EndBlockNode*>(this->children[3]) != nullptr && dynamic_cast<EndBlockNode*>(this->children[3])->getText()=="ENDIF");
    EndBlockNode* endIfNode = dynamic_cast<EndBlockNode*>(this->children[3]);

    BaseNode* child = nullptr;
    if (endIfNode->getChildren().size() == 1) {        //get the single child of the ENDIF node
        child = endIfNode->getChildren()[0];
        endIfNode->removeChild(child);           //remove the child from the ENDIF node, so it doesn't get deleted
        parent->addChildAtIndex(child, indexInParent);
    } 
    //otherwise no children to attach to the parent node, just delete the current node

    // delete the current node from memory
    // delete this;

    //return the child node that has replaced the current node
    return child;
}