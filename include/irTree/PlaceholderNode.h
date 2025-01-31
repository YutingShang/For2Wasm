#pragma once

#include "BaseNode.h"

//an empty node for insertable basic blocks (which are empty new basic blocks, which might get added to the IR tree)
//placeholder is needed so the dataflow analysis can be done on the basic block

//Placeholder node is not actually connected to the IR tree, it is just a placeholder for the basic block

class PlaceholderNode : public BaseNode {

    public:
        PlaceholderNode();
        ~PlaceholderNode() override = default;

        BaseNode* copyNodeOnly() const override;

        virtual void addChild(BaseNode* child) override;   //override to do nothing
        virtual void addChildAtIndex(BaseNode* child, int index) override;   //override to do nothing
        virtual std::string accept(IrBaseVisitor* visitor) override;    //override to throw error - placeholder node should not be in IR tree
        virtual BaseNode* removeCurrentNodeFromIRTree() override;    //override to throw error - placeholder node should not be in IR tree

        virtual std::set<std::string> getReferencedVariables() const override;    //override to return empty set
        virtual std::set<std::string> getDefinedVariables() const override;    //override to return empty set
        virtual std::set<std::string> getReferencedExpressions() const override;    //override to return empty set

};