#include "IfElseNode.h"

IfElseNode::IfElseNode(std::string condition, std::string thenLabel, std::string elseLabel, std::string endLabel) {

    this->textVector = {"IF", condition, thenLabel, elseLabel, endLabel};
}

void IfElseNode::addChild(BaseNode* child) {
    child->setParent(this);

    if (this->children.size() < 4) {
        this->children.push_back(child);
    } else {
        throw std::runtime_error("IfElseNode can only have four children");
    }
}

void IfElseNode::insertChild(BaseNode* child, int index) {
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