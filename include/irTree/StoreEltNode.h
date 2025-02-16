#pragma once

#include "SimpleNode.h"

//For storing an element in an array
//STORE <arrayVar> (<index1>, <index2>, ...) <src - value to store>

class StoreEltNode : public SimpleNode {
public:
    //Constructor and destructor
    StoreEltNode(std::string arrayVar, std::vector<std::string> indices, std::string src);   //the indices are given in fortran (1-based), only converted to 0-based in Wasm
    ~StoreEltNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Getters and setters
    std::string getArrayVar() const;
    std::vector<std::string> getIndicies() const;
    std::string getSrc() const;
    void setArrayVar(std::string arrayVar);
    void setIndicies(std::vector<std::string> indices);
    void setSrc(std::string src);

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

private:
    std::string arrayVar;
    std::vector<std::string> indices;
    std::string src;
    
    
};