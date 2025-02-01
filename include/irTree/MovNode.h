#pragma once
#include "SimpleNode.h"

// Move operation
// MOV <dest> <src>
// Assigns the value of src to dest

class MovNode : public SimpleNode {
public:
    // Constructor and destructor
    MovNode(std::string dest, std::string src);    //set the textVector to be the MOV + dest + src
    ~MovNode() override = default;
    BaseNode* cloneContent() const override;

    // Getters and setters
    std::string getDest() const;
    void setDest(std::string dest) ;
    
    std::string getSrc() const;
    void setSrc(std::string src);

    // Visitor pattern
    std::string accept(IrBaseVisitor* visitor) override;

    // Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;
};