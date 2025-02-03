#include "EntryNode.h"

EntryNode::EntryNode() : SimpleNode("ENTRY") {}

std::shared_ptr<BaseNode> EntryNode::cloneContent() const {
    return std::make_shared<EntryNode>();
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string EntryNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitEntryNode(std::static_pointer_cast<EntryNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> EntryNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> EntryNode::getReferencedExpressions() const {
    return {};
}