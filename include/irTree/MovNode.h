#pragma once
#include "SimpleNode.h"

// Move operation
// MOV <dest> <src>
// Assigns the value of src to dest

class MovNode : public SimpleNode {
public:
    // Constructor and destructor
    MovNode(std::string dest, std::string src);    
    ~MovNode() override = default;
    std::string getText() const override;    //getText to return the MOV + dest + src
    std::shared_ptr<BaseNode> cloneContent() const override;

    // Getters and setters
    std::string getDest() const;
    void setDest(std::string dest) ;
    
    std::string getSrc() const;
    void setSrc(std::string src);

    // Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    // Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;
    void replaceReferencedVariable(std::string oldVar, std::string newVar) override;

private:
    std::string dest;
    std::string src;
};