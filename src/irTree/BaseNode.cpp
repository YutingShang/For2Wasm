#include "BaseNode.h"

BaseNode::BaseNode(std::string text){

    std::istringstream stream(text);
    std::string token;

    while (stream >> token)
    { // split the line into tokens, removing whitespaces
        this->textVector.push_back(token);
    }
    
}

std::string BaseNode::getText() const {
    std::string text = "";
    for (std::string token : this->textVector) {
        text += token + " ";
    }
    return text;
}

// std::string BaseNode::getTextAt(int index) const {
//     return this->textVector[index];
// }

// void BaseNode::addLabel(std::string label) {
//     this->label = label;
// }


std::vector<BaseNode*> BaseNode::getChildren() const {
    return this->children;
}

std::string BaseNode::stringifyIRTree() const {
    std::string tree = "\t" + getText();

    for (BaseNode* child : this->children){
        tree += "\n" + child->stringifyIRTree();
    }
    return tree;
}