#pragma once

#include "SimpleNode.h"

//For loading an element from an array
//LOAD <dest> <arrayVar> (<index1>, <index2>, ...)

class LoadEltNode : public SimpleNode {
public:
    //Constructor and destructor
    LoadEltNode(std::string dest, std::string arrayVar, std::vector<std::string> f_indices);   //the indices are given in fortran (1-based), but converted to IR indices (0-based)
    ~LoadEltNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getDest() const;
    std::string getArrayVar() const;
    std::vector<std::string> getIndicies() const;
    void setDest(std::string dest);
    void setArrayVar(std::string arrayVar);
    void setIndicies(std::vector<std::string> indices);

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

private:
    std::string dest;
    std::string arrayVar;
    std::vector<std::string> indices;
    
};