#include "StoreEltNode.h"

StoreEltNode::StoreEltNode(std::string arrayVar, std::vector<std::string> indices, std::string src) : 
arrayVar(arrayVar), indices(indices), src(src) {}

std::string StoreEltNode::getText() const {
    return "STORE " + arrayVar + " " + IRSemantics::getVectorText(indices) + " " + src;
}

std::shared_ptr<BaseNode> StoreEltNode::cloneContent() const {
    return std::make_shared<StoreEltNode>(arrayVar, indices, src);
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string StoreEltNode::getArrayVar() const {
    return arrayVar;
}

std::vector<std::string> StoreEltNode::getIndicies() const {
    return indices;
}

std::string StoreEltNode::getSrc() const {
    return src;
}

void StoreEltNode::setArrayVar(std::string arrayVar) {
    this->arrayVar = arrayVar;
}

void StoreEltNode::setIndicies(std::vector<std::string> indices) {
    this->indices = indices;
}

void StoreEltNode::setSrc(std::string src) {
    this->src = src;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string StoreEltNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitStoreEltNode(std::dynamic_pointer_cast<StoreEltNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> StoreEltNode::getReferencedVariables() const {
    std::set<std::string> refs;
    if (IRSemantics::isVariable(src)) {
        refs.insert(src);
    }
    // Add any variables used in indices
    for (const auto& index : indices) {
        if (IRSemantics::isVariable(index)) {
            refs.insert(index);
        }
    }

    //arrayVar is actually in the referenced set, not the defined set
    if (IRSemantics::isVariable(arrayVar)) {   //well it definitely should be
        refs.insert(arrayVar);
    }
    return refs;
}

std::set<std::string> StoreEltNode::getDefinedVariables() const {
    return {};    //the whole array has not been defined, only an element of it. Can't say it's defined, otherwise another assignment will make a previous assignment 'dead code'
}

std::set<std::string> StoreEltNode::getReferencedExpressions() const {
    return {};  // arrayVar+ IRSemantics::getVectorText(indices)   //cannot say it's referenced array(indicies), since we are not explicitly loading the array at that index into a temp variable - cannot be reused
}

void StoreEltNode::replaceReferencedVariable(std::string oldVar, std::string newVar) {
    for (auto& index : indices) {
        if (index == oldVar) {
            index = newVar;
        }
    }

    if (src == oldVar) {
        src = newVar;
    }
}










