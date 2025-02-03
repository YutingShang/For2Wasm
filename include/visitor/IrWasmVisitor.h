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

        std::string getMemoryImportCode();

        std::string getEntireProgramCode(const std::shared_ptr<BaseNode>& startNode);

    private:
        bool isPosInteger(const std::string &s);
        bool isTempVariable(const std::string &s);
        bool isStringConst(const std::string &s);

        // src can be string constant, integer, program variables including _s, or internal _t temp variables
        std::string convertSrcToWASM(const std::string &operand, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies);

        // dest can be program variables including _s, or internal _t temp variables
        std::string convertDestToWASM(const std::string &dest);

        ///NOTE: stringMapIndicies is from <str_var> -> array<start_offset, length>
        // e.g. where str_var = $str1, start_offset = 50, length = 4
        std::unordered_map<std::string, std::array<unsigned long, 2>> stringMapIndicies;
       
        //stack contains the endloop labels of the loops
        std::stack<std::string> exitStack;

        bool importPrint = false;
        bool importPrintString = false;
        bool importRead = false;
        std::string memoryImportCode = "";


};