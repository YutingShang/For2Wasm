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

        std::string visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) override;

        std::string getMemoryImportCode();

        std::string getEntireProgramCode(const std::shared_ptr<BaseNode>& startNode);

    private:
        // src can be integer/float constant, or any variable - if temp variable, then just don't print it
        // expectedWasmDatatype (for the operation) might be different from the actual datatype of the operand - e.g. may need to convert or truncate
        std::string convertNumberSrcToWASM(const std::string &operand, std::string expectedWasmDatatype);

        //for string constants, converts $str1 to the array offset and length to be fetched from the wasm memory
        std::string convertStringSrcToWASM(const std::string &str, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies);

        // dest can be program variables including _s, or internal _t temp variables
        std::string convertDestToWASM(const std::string &dest);

        //converts IR datatype to wasm datatype
        std::string convertDatatypeToWASM(const std::string &datatype);

        //very simple helper functions for differentiating i32/i64 , f32/f64
        bool isWASMIntDatatype(const std::string& type);     //i32 or i64
        bool isWASMFloatDatatype(const std::string& type);   //f32 or f64

        //finds the largest datatype between two types - e.g. if trying to operate on i32 and f32, then returns f32
        std::string findLargestDatatype(const std::string& type1, const std::string& type2);

        //get the wasm number datatype of a variable or constant
        //typically used for finding the datatype of a nodeSrc. Can then use findLargestDatatype 
        std::string getWASMNumberDatatype(const std::string& item);    //e.g. input x or _t0 or 3.14, returns i32 or f32 etc.

        ///NOTE: stringMapIndicies is from <str_var> -> array<start_offset, length>
        // e.g. where str_var = $str1, start_offset = 50, length = 4
        std::unordered_map<std::string, std::array<unsigned long, 2>> stringMapIndicies;

        //maps all variables (including internal temp variables) to their WASM datatype (i32, i64, f32, f64)
        //initially add PROGRAM VARIABLES to the map when visiting DECLARE statements
        //then add INTERNAL TEMP VARIABLES to the map when visiting other statements and creating those temporary variables
        std::unordered_map<std::string, std::string> variableDatatypeMap;
       
        //stack contains the endloop labels of the loops
        std::stack<std::string> exitStack;

        bool importPrint_i32 = false;
        bool importPrint_i64 = false;
        bool importPrint_f32 = false;
        bool importPrint_f64 = false;
        bool importPrintString = false;
        bool importRead = false;
        std::string memoryImportCode = "";

        //return wasm code to save a temporary variable into the tempSwap wasm local variable - so we need to fetch it when we want to use this temp variable
        //currently supports binary operations (could expand for 3, 4, etc. operands)
        std::string checkAndSaveTempVarToLocal(std::string src1, std::string src2);    

        bool needToRestoreSwappedTemporary = false;    //set to true when we have stored a temporary variable into the tempSwap wasm local variable

        bool addTempSwapDeclaration_i32 = false;  //hopefully just one variable of each type needs to be added to the top declaration section if required
        bool addTempSwapDeclaration_i64 = false;
        bool addTempSwapDeclaration_f32 = false;
        bool addTempSwapDeclaration_f64 = false;

};