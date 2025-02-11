#include "EndBlockNode.h"

EndBlockNode::EndBlockNode(std::string endBlockText) {
    if (!IRSemantics::isValidEndBlock(endBlockText)) {
        throw std::runtime_error("Invalid end block: " + endBlockText);
    }
    this->endBlockText = endBlockText;
}

std::string EndBlockNode::getText() const {
    return endBlockText;
}

std::shared_ptr<BaseNode> EndBlockNode::cloneContent() const {
    return std::make_shared<EndBlockNode>(getText());
}

/////////////////////////VISITOR PATTERN/////////////////////////

std::string EndBlockNode::accept(IrBaseVisitor& visitor) {
    return visitor.visitEndBlockNode(std::static_pointer_cast<EndBlockNode>(shared_from_this()));
}

/////////////////////////ANALYSIS METHODS/////////////////////////

std::set<std::string> EndBlockNode::getReferencedVariables() const {
    return {};
}

std::set<std::string> EndBlockNode::getDefinedVariables() const {
    return {};
}

std::set<std::string> EndBlockNode::getReferencedExpressions() const {
    return {};
}