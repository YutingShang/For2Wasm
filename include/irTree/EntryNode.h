#pragma once

#include "SimpleNode.h"


//similar to ExitNode, has no body 

class EntryNode : public SimpleNode {
public:
    //Constructor and destructor
    EntryNode() = default;
    ~EntryNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    
}; 