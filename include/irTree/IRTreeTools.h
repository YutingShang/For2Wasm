#pragma once
#include "BaseNode.h"
#include <stack>
#include <regex>

class IRTreeTools
{
public:
    std::string irTreeToDotTree(BaseNode *root);

    ///NOTE: stringMap here is from <str_var> -> <string_to_print> (without quotes)
    // e.g. where str_var = $str1, string_to_print = "Add:"
    std::string irTreeToWASM(BaseNode *root, std::unordered_map<std::string, std::string> &stringMap);    //convert the entire IR tree to WASM code

private:

    ///NOTE: stringMapIndicies is from <str_var> -> array<start_offset, length>
    // e.g. where str_var = $str1, start_offset = 50, length = 4
    // convert individual node instructions to WASM
    void processToWASM(BaseNode *nodeToProcess, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies, std::vector<std::string> &exitStack, std::string &wasmCode);
    
    void visitWASM(BaseNode *nodeToProcess, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies, std::vector<std::string> &exitStack, std::string &outputString);

    std::string convertOperandToWASM(const std::string &operand, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies);

    bool isStringConst(const std::string &s);

    bool isPosInteger(const std::string &s);

    bool isTempVariable(const std::string &s);

    bool importPrint = false;
    bool importPrintString = false;
    bool importRead = false;


};