#include "ExitNode.h"

ExitNode::ExitNode() 
    : SimpleNode("EXIT") {
} 

std::shared_ptr<BaseNode> ExitNode::cloneContent() const {
    return std::make_shared<ExitNode>();
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string ExitNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitExitNode(std::static_pointer_cast<ExitNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> ExitNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> ExitNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> ExitNode::getReferencedExpressions() const {
    return {};
}