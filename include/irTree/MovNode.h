#pragma once
#include "SimpleNode.h"

class MovNode : public SimpleNode {
public:
    MovNode(std::string dest, std::string src);    //set the textVector to be the MOV + dest + src

    ~MovNode() override = default;

    std::string getDest() const;

    std::string getSrc() const;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    // void setDest(std::string dest);
    void setSrc(std::string src);
};