#pragma once

#include "IrBaseVisitor.h"
#include "BasicBlock.h"
#include <stack>

class SimpleNode;

class IrFlowgraphVisitor : public IrBaseVisitor {

    public:

        IrFlowgraphVisitor(BasicBlock* startBasicBlock);

        ~IrFlowgraphVisitor() = default;

        std::string visitArithOpNode(ArithOpNode* node) override;

        std::string visitLogicBinOpNode(LogicBinOpNode* node) override;

        std::string visitLogicNotNode(LogicNotNode* node) override;

        std::string visitRelOpNode(RelOpNode* node) override;

        std::string visitMovNode(MovNode* node) override;

        std::string visitEndBlockNode(EndBlockNode* node) override;

        std::string visitExitNode(ExitNode* node) override;

        std::string visitLoopNode(LoopNode* node) override;

        std::string visitIfNode(IfNode* node) override;

        std::string visitIfElseNode(IfElseNode* node) override;
        
        std::string visitDeclareNode(DeclareNode* node) override;

        std::string visitPrintNode(PrintNode* node) override;

        std::string visitReadNode(ReadNode* node) override;

        std::string visitTestNode(TestNode* node) override;

        std::string visitEntryNode(EntryNode* node) override;



    private:
        std::string visitSimpleNode(SimpleNode* node);

        BasicBlock* currentBasicBlock;

        // Creates a new basic block as the successor of the current basic block and updates the current basic block
        void startNewBasicBlockSuccessor();

        //stack ENDLOOP basic blocks, i.e. the exitLabel of the loop
        std::stack<BasicBlock*> exitStack;  


};