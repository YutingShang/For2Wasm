#pragma once

#include "SimpleNode.h"

class PrintNode : public SimpleNode {
public:
    //Constructor and destructor
    PrintNode(std::string src);
    ~PrintNode() override = default;
    BaseNode* cloneContent() const override;

    //Getters and setters
    std::string getSrc() const;
    void setSrc(std::string src);
    
    //Visitor pattern
    std::string accept(IrBaseVisitor* visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;
};