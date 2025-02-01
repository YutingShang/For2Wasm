#include "IfNode.h"
#include "IfElseNode.h"

IfNode::IfNode(std::string condition, std::string thenLabel, std::string endLabel) {

    this->textVector = {"IF", condition, thenLabel, endLabel};
}

BaseNode* IfNode::cloneContent() const {
    return new IfNode(textVector[1], textVector[2], textVector[3]);
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
    // delete this;

    //return the child node that has replaced the current node
    return child;
}

IfElseNode* IfNode::convertToIfElseNode() {

    //print the current node's children
    // std::cout <<"current node's children: " <<this->getText() <<std::endl;

    // std::cout <<"current node's tree: " <<this->stringifyIRTree() <<std::endl;

    //create a new IfElseNode with the same condition, thenLabel, and endLabel
    std::string thenLabel = this->textVector[2];     //get the label suffix number from the thenLabel
    std::string labelSuffixNumber;

    size_t firstDigit = thenLabel.find_first_of("0123456789");
    if (firstDigit != std::string::npos) {
        labelSuffixNumber = thenLabel.substr(firstDigit);
    }

    std::string elseLabel = "else" + labelSuffixNumber;
    IfElseNode* ifElseNode = new IfElseNode(this->textVector[1], this->textVector[2], elseLabel, this->textVector[3]);
    
    BaseNode* conditionChild = this->children[0];
    // std::cout <<"conditionChild: " <<conditionChild->getText() <<std::endl;
    BaseNode* thenChild = this->children[1];
    // std::cout <<"thenChild: " <<thenChild->getText() <<std::endl;
    BaseNode* endIfChild = this->children[2];
    // std::cout <<"endIfChild: " <<endIfChild->getText() <<std::endl;

    this->removeChild(conditionChild);
    this->removeChild(thenChild);
    this->removeChild(endIfChild);
    
    //then we move the children of the current node to the new node
    ifElseNode->addChild(conditionChild);    //condition
    ifElseNode->addChild(thenChild);    //then block

    // create a new ELSE block
    EndBlockNode *endElseNode = new EndBlockNode("ENDELSE");
    ifElseNode->addChild(endElseNode);          //else block
    ifElseNode->addChild(endIfChild);    //endif block

    //get the current node's parent and add the new node to the parent
    BaseNode* parent = this->getParent();
    parent->removeChild(this);
    parent->addChild(ifElseNode);

    ///WARNING: deleting the current node from memory - maybe not?
    // delete this;
    
    return ifElseNode;
}