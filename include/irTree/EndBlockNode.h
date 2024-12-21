#pragma once
#include "SimpleNode.h"

// EndBlockNode is a class for nodes that represent the end of a block 
// ENDTHEN ENDTHEN ENDIF ENDLOOP

class EndBlockNode : public SimpleNode {
public:
    EndBlockNode(std::string text);        //text is ENDTHEN ENDTHEN ENDIF ENDLOOP etc.
    ~EndBlockNode() override = default;

    std::string accept(IrBaseVisitor* visitor) override;

    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
};