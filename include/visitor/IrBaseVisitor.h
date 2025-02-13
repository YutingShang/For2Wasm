#pragma once

#include <string>
//forward declarations
class ArithOpNode;
class LogicBinOpNode;
class LogicNotNode;
class RelOpNode;
class MovNode;
class EndBlockNode;
class ExitNode;
class LoopNode;
class LoopCondNode;
class IfNode;
class DeclareNode;
class PrintNode;
class ReadNode;
class BaseNode;
class EntryNode;
class IfElseNode;
class TestNode;
class DeclareArrayNode;
class DataArrayNode;
class StoreEltNode;
class LoadEltNode;

// a visitor interface for the IR tree, visit methods will be used in double dispatch in the nodes 'accept' method

class IrBaseVisitor {
    public:
        virtual ~IrBaseVisitor() = default;
        virtual std::string visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) = 0;

        virtual std::string visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) = 0;

        virtual std::string visitRelOpNode(const std::shared_ptr<RelOpNode>& node) = 0;

        virtual std::string visitMovNode(const std::shared_ptr<MovNode>& node) = 0;

        virtual std::string visitEndBlockNode(const std::shared_ptr<EndBlockNode>& node) = 0;

        virtual std::string visitExitNode(const std::shared_ptr<ExitNode>& node) = 0;

        virtual std::string visitLoopNode(const std::shared_ptr<LoopNode>& node) = 0;

        virtual std::string visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) = 0;

        virtual std::string visitIfNode(const std::shared_ptr<IfNode>& node) = 0;

        virtual std::string visitIfElseNode(const std::shared_ptr<IfElseNode>& node) = 0;

        virtual std::string visitDeclareNode(const std::shared_ptr<DeclareNode>& node) = 0;

        virtual std::string visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) = 0;

        virtual std::string visitPrintNode(const std::shared_ptr<PrintNode>& node) = 0;

        virtual std::string visitReadNode(const std::shared_ptr<ReadNode>& node) = 0;

        virtual std::string visitTestNode(const std::shared_ptr<TestNode>& node) = 0;

        virtual std::string visitEntryNode(const std::shared_ptr<EntryNode>& node) = 0;

        virtual std::string visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) = 0;

        virtual std::string visitDataArrayNode(const std::shared_ptr<DataArrayNode>& node) = 0;

        virtual std::string visitStoreEltNode(const std::shared_ptr<StoreEltNode>& node) = 0;

        virtual std::string visitLoadEltNode(const std::shared_ptr<LoadEltNode>& node) = 0;
};