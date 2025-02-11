#pragma once
#include "SimpleNode.h"

// EndBlockNode is a class for nodes that represent the end of a block 
// ENDTHEN ENDELSE ENDIF ENDLOOP ENDBODY

class EndBlockNode : public SimpleNode {
public:
    //Constructor and destructor
    EndBlockNode(std::string endBlockText);        //text is ENDTHEN ENDTHEN ENDIF ENDLOOP ENDBODY etc.
    ~EndBlockNode() override = default;
    std::string getText() const override;
    std::shared_ptr<BaseNode> cloneContent() const override;

    //Visitor pattern
    std::string accept(IrBaseVisitor& visitor) override;

    //Analysis methods
    std::set<std::string> getReferencedVariables() const override;
    std::set<std::string> getDefinedVariables() const override;
    std::set<std::string> getReferencedExpressions() const override;

private:
    std::string endBlockText;
};