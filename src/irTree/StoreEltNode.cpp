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
    return refs;
}

std::set<std::string> StoreEltNode::getDefinedVariables() const {
    return {getArrayVar()};
}

std::set<std::string> StoreEltNode::getReferencedExpressions() const {
    return {};
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










