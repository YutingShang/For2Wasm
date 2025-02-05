#pragma once

#include "IrBaseVisitor.h"
#include "BasicBlock.h"
#include <stack>

class SimpleNode;

class IrFlowgraphVisitor : public IrBaseVisitor {

    public:

        IrFlowgraphVisitor(std::shared_ptr<BasicBlock> startBasicBlock);

        ~IrFlowgraphVisitor() = default;

        std::string visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) override;

        std::string visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) override;

        std::string visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) override;

        std::string visitRelOpNode(const std::shared_ptr<RelOpNode>& node) override;

        std::string visitMovNode(const std::shared_ptr<MovNode>& node) override;

        std::string visitEndBlockNode(const std::shared_ptr<EndBlockNode>& node) override;

        std::string visitExitNode(const std::shared_ptr<ExitNode>& node) override;

        std::string visitLoopNode(const std::shared_ptr<LoopNode>& node) override;

        std::string visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) override;

        std::string visitIfNode(const std::shared_ptr<IfNode>& node) override;

        std::string visitIfElseNode(const std::shared_ptr<IfElseNode>& node) override;
        
        std::string visitDeclareNode(const std::shared_ptr<DeclareNode>& node) override;

        std::string visitPrintNode(const std::shared_ptr<PrintNode>& node) override;

        std::string visitReadNode(const std::shared_ptr<ReadNode>& node) override;

        std::string visitTestNode(const std::shared_ptr<TestNode>& node) override;

        std::string visitEntryNode(const std::shared_ptr<EntryNode>& node) override;



    private:
        std::string visitSimpleNode(const std::shared_ptr<SimpleNode>& node);

        std::shared_ptr<BasicBlock> currentBasicBlock;

        // Creates a new basic block as the successor of the current basic block and updates the current basic block
        void startNewBasicBlockSuccessor();

        //stack ENDLOOP basic blocks, i.e. the endloopLabel of the loop
        std::stack<std::shared_ptr<BasicBlock>> exitStack;  


};