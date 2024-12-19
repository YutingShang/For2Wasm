#pragma once

#include "IrBaseVisitor.h"
#include <stack>
class IrWasmVisitor : public IrBaseVisitor {

    public:

        ///NOTE: stringMap here is from <str_var> -> <string_to_print> (without quotes)
        // e.g. where str_var = $str1, string_to_print = "Add:"
        ///NOTE: we convert the stringMap to stringMapIndicies in the constructor
        ///NOTE: constructor also creates the memoryImportCode, which is part of the header WASM code
        IrWasmVisitor(std::unordered_map<std::string, std::string> &stringMap);

        ~IrWasmVisitor() = default;

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

        std::string getMemoryImportCode();

        std::string getEntireProgramCode(BaseNode* startNode);

    private:
        bool isPosInteger(const std::string &s);
        bool isTempVariable(const std::string &s);
        bool isStringConst(const std::string &s);
        std::string convertOperandToWASM(const std::string &operand, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies);

        ///NOTE: stringMapIndicies is from <str_var> -> array<start_offset, length>
        // e.g. where str_var = $str1, start_offset = 50, length = 4
        std::unordered_map<std::string, std::array<unsigned long, 2>> stringMapIndicies;
       
        std::stack<std::string> exitStack;

        bool importPrint = false;
        bool importPrintString = false;
        bool importRead = false;
        std::string memoryImportCode = "";


};