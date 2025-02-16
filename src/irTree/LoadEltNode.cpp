#include "LoadEltNode.h"

LoadEltNode::LoadEltNode(std::string dest, std::string arrayVar, std::vector<std::string> indices) : 
dest(dest), arrayVar(arrayVar), indices(indices) {}

std::string LoadEltNode::getText() const {
    return "LOAD " + dest + " " + arrayVar + " " + IRSemantics::getVectorText(indices);
}

std::shared_ptr<BaseNode> LoadEltNode::cloneContent() const {
    return std::make_shared<LoadEltNode>(dest, arrayVar, indices);
}

/////////////////////////GETTERS AND SETTERS/////////////////////////

std::string LoadEltNode::getDest() const {
    return dest;
}

std::string LoadEltNode::getArrayVar() const {
    return arrayVar;
}

std::vector<std::string> LoadEltNode::getIndicies() const {
    return indices;
}

void LoadEltNode::setDest(std::string dest) {
    this->dest = dest;
}

void LoadEltNode::setArrayVar(std::string arrayVar) {
    this->arrayVar = arrayVar;
}

void LoadEltNode::setIndicies(std::vector<std::string> indices) {
    this->indices = indices;
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string LoadEltNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitLoadEltNode(std::dynamic_pointer_cast<LoadEltNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> LoadEltNode::getReferencedVariables() const {
    std::set<std::string> refs;
    if (IRSemantics::isVariable(arrayVar)) {   //well it definitely should be
        refs.insert(arrayVar);
    }
    for (const auto& index : indices) {
        if (IRSemantics::isVariable(index)) {
            refs.insert(index);
        }
    }
    return refs;
}

std::set<std::string> LoadEltNode::getDefinedVariables() const {
    return {dest};
}

std::set<std::string> LoadEltNode::getReferencedExpressions() const {
    return {};
}
