#pragma once

#include "IrBaseVisitor.h"
#include <map>

class IrNestedLoopVisitor : public IrBaseVisitor {
    public:
        IrNestedLoopVisitor();
        ~IrNestedLoopVisitor() = default;

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

        std::string visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) override;

        std::string visitDataArrayNode(const std::shared_ptr<DataArrayNode>& node) override;

        std::string visitStoreEltNode(const std::shared_ptr<StoreEltNode>& node) override;

        std::string visitLoadEltNode(const std::shared_ptr<LoadEltNode>& node) override;

        std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> getPerfectlyNestedLoops();


    private:

        //map of an OUTER loopCondNode to a vector of its perfectly nested INNER loopCondNodes (if there are multiple, they are nested; if there are none, its just an outer loop - can still do loop stripmining)
        std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> perfectlyNestedLoops;

        // bool isOuterLoop = true;      //once we go inside a loop, we set this to false

        //number of loops nested (may not be perfectly nested, or could be LoopNode, but just needs to pair up with ENDLOOP)
        //when numberOfLoopsNested is 0, we are outside all loops - so next loop would be an outer loop
        int numberOfLoopsNested = 0;

        //this is one of the conditions for a perfectly nested loop
        bool parentIsLoopCondNode = false;

        std::shared_ptr<LoopCondNode> currentOuterLoopCondNode;

        std::string acceptAllChildren(const std::shared_ptr<BaseNode>& node);

        
};