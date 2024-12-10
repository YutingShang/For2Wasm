#pragma once
#include "SimpleNode.h"


// Nodes in this class are part of the IR tree purely for annotation purposes
// They have no equivalent in the wasm code generation
// e.g. TEST <var> and ENTRY

class GhostNode : public SimpleNode {

    public:
        GhostNode(std::string text);      //set the text, can basically ignore it
        ~GhostNode() override = default;

        std::string accept(IrBaseVisitor* visitor) override;     

};