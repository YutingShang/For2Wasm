#include "EndBlockNode.h"

EndBlockNode::EndBlockNode(std::string text) {
    if (!isValidEndBlock(text)) {
        throw std::runtime_error("Invalid end block: " + text);
    }
    textVector = {text};
}

std::shared_ptr<BaseNode> EndBlockNode::cloneContent() const {
    return std::make_shared<EndBlockNode>(getText());
}

bool EndBlockNode::isValidEndBlock(std::string text) {
    return text == "ENDTHEN" || text == "ENDELSE" || text == "ENDIF" || text == "ENDLOOP" || text == "ENDBODY";
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