#pragma once

#include "BaseNode.h"

//an empty node for insertable basic blocks (which are empty new basic blocks, which might get added to the IR tree)
//placeholder is needed so the dataflow analysis can be done on the basic block

//Placeholder node is not actually connected to the IR tree, it is just a placeholder for the basic block

class PlaceholderNode : public BaseNode {
public:
    //Constructor and destructor
    PlaceholderNode() = default;
    ~PlaceholderNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Visitor pattern - override to throw error - placeholder node should not be in IR tree
    virtual std::string accept(IrBaseVisitor& visitor) override;    

    //Analysis methods - override to return empty set
    virtual std::set<std::string> getReferencedVariables() const override;    
    virtual std::set<std::string> getDefinedVariables() const override;    
    virtual std::set<std::string> getReferencedExpressions() const override;    

    //Tree manipulation methods - override to do nothing or throw error - placeholder node should not be in IR tree
    virtual void addChild(std::shared_ptr<BaseNode> child) override;   
    virtual void addChildAtIndex(std::shared_ptr<BaseNode> child, int index) override;   
    virtual std::shared_ptr<BaseNode> removeCurrentNodeFromIRTree() override;    
};