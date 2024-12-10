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
class IfNode;
class GhostNode;
class DeclareNode;
class PrintNode;
class ReadNode;
class BaseNode;

// a visitor interface for the IR tree, visit methods will be used in double dispatch in the nodes 'accept' method

class IrBaseVisitor {
    public:
        virtual ~IrBaseVisitor() = default;

        virtual std::string visitArithOpNode(ArithOpNode* node) = 0;

        virtual std::string visitLogicBinOpNode(LogicBinOpNode* node) = 0;

        virtual std::string visitRelOpNode(RelOpNode* node) = 0;

        virtual std::string visitMovNode(MovNode* node) = 0;

        virtual std::string visitEndBlockNode(EndBlockNode* node) = 0;

        virtual std::string visitExitNode(ExitNode* node) = 0;

        virtual std::string visitLoopNode(LoopNode* node) = 0;

        virtual std::string visitIfNode(IfNode* node) = 0;

        virtual std::string visitGhostNode(GhostNode* node) = 0;

        virtual std::string visitDeclareNode(DeclareNode* node) = 0;

        virtual std::string visitLogicNotNode(LogicNotNode* node) = 0;

        virtual std::string visitPrintNode(PrintNode* node) = 0;

        virtual std::string visitReadNode(ReadNode* node) = 0;
};