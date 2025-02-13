#include "IrWasmVisitor.h"

#include "IRSemantics.h"
#include "ArithOpNode.h"
#include "LogicBinOpNode.h"
#include "LogicNotNode.h"
#include "RelOpNode.h"
#include "MovNode.h"
#include "EndBlockNode.h"
#include "ExitNode.h"
#include "LoopNode.h"
#include "LoopCondNode.h"
#include "IfNode.h"
#include "IfElseNode.h"
#include "TestNode.h"
#include "EntryNode.h"
#include "DeclareNode.h"
#include "PrintNode.h"
#include "ReadNode.h"
#include "BaseNode.h"
#include "DeclareArrayNode.h"
#include "IrTypeVisitor.h"
#include <iostream>

IrWasmVisitor::IrWasmVisitor(std::unordered_map<std::string, std::string> &stringMap, std::unordered_map<std::string, std::string> &irDatatypeMap){
    // constructor that converts stringMap to stringMapIndicies
    unsigned long base_offset = 0;
    bool importedMemory = false; // only import memory once, and only if there is at least one string
   
    for (auto &entry : stringMap)
    {
        if (!importedMemory) {
            memoryImportCode = "(import \"js\" \"mem\" (memory 1))\n";
            importedMemory = true;
        }
        std::string string_to_print = entry.second;
        //string constants using i32 to specify the offset
        memoryImportCode += "(data (i32.const " + std::to_string(base_offset) + ") \"" + string_to_print + "\")" + "\n";
        stringMapIndicies[entry.first] = {base_offset, string_to_print.length()};
        base_offset += string_to_print.length();
    }

    //convert irDatatypeMap to variableWASMDatatypeMap
    for (auto &entry : irDatatypeMap) {
        variableWASMDatatypeMap[entry.first] = convertDatatypeToWASM(entry.second);
    }
}

std::string IrWasmVisitor::getMemoryImportCode() {
    return memoryImportCode;
}

std::string IrWasmVisitor::getEntireProgramCode(const std::shared_ptr<BaseNode>& startNode) {
    
    std::string moduleHeader = "(module\n";
    std::string funcHeader = "(func (export \"main\")\n";
    std::string mainCode = startNode->accept(*this);
    mainCode += ")\n)";     //to close the module and main function

    // after the main code, flags will be set, so we can add the imports
    if (importPrint_i32) {
        moduleHeader += "(import \"console\" \"log\" (func $log_i32 (param i32)))\n";
    }
    if (importPrint_i64) {
        moduleHeader += "(import \"console\" \"log_i64\" (func $log_i64 (param i64)))\n";   //this one needs special JS function, others can simply use console.log
    }
    if (importPrint_f32) {
        moduleHeader += "(import \"console\" \"log\" (func $log_f32 (param f32)))\n";
    }
    if (importPrint_f64) {
        moduleHeader += "(import \"console\" \"log\" (func $log_f64 (param f64)))\n";
    }
    if (importPrintString) {
        moduleHeader += "(import \"console\" \"logString\" (func $logString (param i32 i32)))\n";
    }
    if (importRead) {
        moduleHeader += "(import \"console\" \"promptSync\" (func $read (result i32)))\n";
    }

    if (addTempSwapDeclaration_i32) {
        funcHeader += "(local $tempSwap_i32 i32)\n";
    }
    if (addTempSwapDeclaration_i64) {
        funcHeader += "(local $tempSwap_i64 i64)\n";
    }
    if (addTempSwapDeclaration_f32) {
        funcHeader += "(local $tempSwap_f32 f32)\n";
    }
    if (addTempSwapDeclaration_f64) {
        funcHeader += "(local $tempSwap_f64 f64)\n";
    }
    std::string overallCode = moduleHeader + getMemoryImportCode() + funcHeader + mainCode;
    return overallCode;
}

std::string IrWasmVisitor::visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) {
    std::string nodeDest = node->getDest();
    std::string nodeSrc1 = node->getSrc1();
    std::string nodeSrc2 = node->getSrc2();
    std::string arithOp = node->getOp();
    
    std::string expectedWasmDatatype;
    if (IRSemantics::isVariable(nodeDest)) {
        expectedWasmDatatype = variableWASMDatatypeMap[nodeDest];    //if there is a concrete dest, use that datatype
    }
    else{
        throw std::runtime_error("Invalid destination in visitArithOpNode: "+nodeDest);
    }

    std::string wasmCode = "";
    if (arithOp == "SUB" || arithOp == "DIV"){   //non-commutative operations
        wasmCode += checkAndSaveTempVarToLocal(nodeSrc1, nodeSrc2);   //need to check whether you need to swap operands (i.e. save then restore) on the stack
    }
    wasmCode += convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);

    
    wasmCode += expectedWasmDatatype + ".";     //e.g. i32.add
    if (arithOp =="ADD"){
        wasmCode += "add\n";
    }
    else if (arithOp == "SUB"){
        wasmCode += "sub\n";
    }
    else if (arithOp == "MUL"){
        wasmCode += "mul\n";
    }
    else if (arithOp == "DIV"){
        if (isWASMIntDatatype(expectedWasmDatatype)){
            wasmCode += "div_s\n";   //signed division!? (hmm doesn't assume positive numbers? or just consistent with stuff - check edge cases)
        }
        else{
            wasmCode += "div\n";    
        }
    }

    wasmCode += convertDestToWASM(nodeDest);

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;

}

std::string IrWasmVisitor::visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) {
    std::string nodeDest = node->getDest();
    std::string nodeSrc1 = node->getSrc1();
    std::string nodeSrc2 = node->getSrc2();
    std::string expectedWasmDatatype;
    if (IRSemantics::isVariable(nodeDest)) {
        expectedWasmDatatype = variableWASMDatatypeMap[nodeDest];
    }
    else{
        throw std::runtime_error("Invalid destination in visitLogicBinOpNode: "+nodeDest);
    }
    std::string wasmCode = convertNumberSrcToWASM(node->getSrc1(), expectedWasmDatatype) + convertNumberSrcToWASM(node->getSrc2(), expectedWasmDatatype);
    std::string logicOp = node->getOp();
    wasmCode += expectedWasmDatatype + ".";     //e.g. i32.and
    if (logicOp == "AND")
        wasmCode += "and\n";
    else if (logicOp == "OR")
        wasmCode += "or\n";

    wasmCode += convertDestToWASM(nodeDest);

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) {
    std::string nodeDest = node->getDest();
    std::string nodeSrc = node->getSrc();
    std::string expectedWasmDatatype;
    if (IRSemantics::isVariable(nodeDest)) {
        expectedWasmDatatype = variableWASMDatatypeMap[nodeDest];
    }
    else{
        throw std::runtime_error("Invalid destination in visitLogicNotNode: "+nodeDest);
    }

    std::string wasmCode = convertNumberSrcToWASM(nodeSrc, expectedWasmDatatype);
    wasmCode += expectedWasmDatatype + ".eqz\n";      //e.g. i32.eqz
    wasmCode += convertDestToWASM(nodeDest);

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitRelOpNode(const std::shared_ptr<RelOpNode>& node) {
    std::string nodeDest = node->getDest();
    std::string nodeSrc1 = node->getSrc1();
    std::string nodeSrc2 = node->getSrc2();
    std::string relOp = node->getOp();
    std::string expectedWasmDatatype;
    if (IRSemantics::isVariable(nodeDest)) {
        expectedWasmDatatype = variableWASMDatatypeMap[nodeDest];
    }
    else{
        throw std::runtime_error("Invalid destination in visitRelOpNode: "+nodeDest);
    }
    std::string wasmCode = "";
    if (relOp == "LT" || relOp == "GT" || relOp == "LE" || relOp == "GE"){  //non-commutative operations
        wasmCode += checkAndSaveTempVarToLocal(nodeSrc1, nodeSrc2);   //need to check whether you need to swap operands on the stack
    }
    wasmCode += convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);

    wasmCode += expectedWasmDatatype + ".";     //e.g. i32.eq
    if (relOp == "EQ")
        wasmCode += "eq\n";
    else if (relOp == "NE")
        wasmCode += "ne\n";
    else if (relOp == "LT")
        wasmCode += "lt_s\n";
    else if (relOp == "GT")
        wasmCode += "gt_s\n";
    else if (relOp == "LE")
        wasmCode += "le_s\n";
    else if (relOp == "GE")
        wasmCode += "ge_s\n";

    wasmCode += convertDestToWASM(nodeDest);

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitMovNode(const std::shared_ptr<MovNode>& node) {
    std::string nodeDest = node->getDest();
    std::string nodeSrc = node->getSrc();
    std::string expectedWasmDatatype;
    if (IRSemantics::isVariable(nodeDest)) {
        expectedWasmDatatype = variableWASMDatatypeMap[nodeDest];
    }
    else{
        throw std::runtime_error("Invalid destination in visitMovNode: "+nodeDest);
    }
    std::string wasmCode = convertNumberSrcToWASM(nodeSrc, expectedWasmDatatype);
    wasmCode += convertDestToWASM(nodeDest);
   
    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitEndBlockNode(const std::shared_ptr<EndBlockNode>& node) {
    std::string wasmCode = "";
    //DON'T add ) if it is ENDBODY, since we add br $bodyLabel when we process the ENDLOOP node
    if (node->getText() != "ENDBODY") {    
        wasmCode = ")\n";
    }

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitExitNode(const std::shared_ptr<ExitNode>& node) {
    std::string endloopLabel = exitStack.top();
    exitStack.pop();
    std::string wasmCode = "br $" + endloopLabel + "\n";

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitLoopNode(const std::shared_ptr<LoopNode>& node) {
    std::string wasmCode = "(block $" + node->getEndloopLabel() + "\n";
    wasmCode += "(loop $" + node->getBodyLabel() + "\n";
    exitStack.push(node->getEndloopLabel());
    wasmCode += node->getChildren()[0]->accept(*this);    // process the body of the loop
    wasmCode += "br $" + node->getBodyLabel() + "\n)\n";     // for end of one of the block statement
    
    std::string endloopCode = node->getChildren()[1]->accept(*this);
    return wasmCode + endloopCode;
}

std::string IrWasmVisitor::visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) {
    std::string wasmCode = "(block $" + node->getEndLoopLabel() + "\n";
    
    //sandwich the initialisation code between the block and the loop - unnecessary but might make it clearer it is the initialisation logic
    std::string initCode = node->getChildren()[0]->accept(*this);       
    wasmCode += initCode;

    wasmCode += "(loop $" + node->getBodyLabel() + "\n";
    std::string conditionCode = node->getChildren()[1]->accept(*this);
    //does not need to use exitStack here, since we know where the EXIT is 
    //we are adding it at the start of the loop, after the termination condition check
    std::string extraTerminationCode = "(if \n(then \n br $" + node->getEndLoopLabel() + "\n)\n)\n";
    std::string bodyCode = node->getChildren()[2]->accept(*this);
    std::string stepCode = node->getChildren()[3]->accept(*this);
    std::string extraEndloopCode = "br $" + node->getBodyLabel() + "\n)\n";
    std::string afterEndNodeCode = node->getChildren()[4]->accept(*this);
    wasmCode += conditionCode + extraTerminationCode + bodyCode + stepCode + extraEndloopCode + afterEndNodeCode;
    return wasmCode;
}

std::string IrWasmVisitor::visitIfNode(const std::shared_ptr<IfNode>& node) {
    std::string conditionCode = node->getChildren()[0]->accept(*this);
    std::string thenCode = node->getChildren()[1]->accept(*this);

    
    std::string endCode = node->getChildren()[2]->accept(*this);
    return conditionCode + "(if\n(then\n" + thenCode + endCode;
}

std::string IrWasmVisitor::visitIfElseNode(const std::shared_ptr<IfElseNode>& node) {
    std::string conditionCode = node->getChildren()[0]->accept(*this);
    std::string thenCode = node->getChildren()[1]->accept(*this);

    std::string elseCode = node->getChildren()[2]->accept(*this);
    std::string endCode = node->getChildren()[3]->accept(*this);    //also contains rest of code
    return conditionCode + "(if\n(then\n" + thenCode + "(else\n" + elseCode + endCode;
}

std::string IrWasmVisitor::visitDeclareNode(const std::shared_ptr<DeclareNode>& node) {
    std::string wasmDatatype = convertDatatypeToWASM(node->getDatatype());   //convert IR datatype to WASM datatype
    std::string nodeVar = node->getVar();    

    std::string wasmCode = "(local $" + nodeVar + " " + wasmDatatype + ")\n";
    
    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) {
    ///TODO: implement generate WASM code for array declaration
    return "generate WASM code for array declaration";
}

std::string IrWasmVisitor::visitPrintNode(const std::shared_ptr<PrintNode>& node) {
    std::string nodeSrc = node->getSrc();
    std::string wasmCode = "";
    if (IRSemantics::isStringConstant(nodeSrc)) {
        wasmCode += convertStringSrcToWASM(nodeSrc, stringMapIndicies);
    }
    else{ //its a number (const or variable)
        wasmCode += convertNumberSrcToWASM(nodeSrc, getWASMNumberDatatype(nodeSrc));    //??? need to check src datatype
    }

    ///TODO: handle different datatypes for printing
    if (IRSemantics::isStringConstant(nodeSrc)) {
        wasmCode += "call $logString\n";
        importPrintString = true;
    } else {
        std::string numDatatype = getWASMNumberDatatype(nodeSrc);
        if (numDatatype == "i32"){
            wasmCode += "call $log_i32\n";
            importPrint_i32 = true;
        }
        else if (numDatatype == "i64"){
            wasmCode += "call $log_i64\n";
            importPrint_i64 = true;
        }
        else if (numDatatype == "f32"){
            wasmCode += "call $log_f32\n";
            importPrint_f32 = true;
        }
        else if (numDatatype == "f64"){
            wasmCode += "call $log_f64\n";
            importPrint_f64 = true;
        }
        else{
            throw std::runtime_error("Invalid datatype in visitPrintNode: "+numDatatype);
        }
    }

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitReadNode(const std::shared_ptr<ReadNode>& node) {
    std::string wasmCode = "call $read\n";
    wasmCode += "local.set $" + node->getVar() + "\n";
    importRead = true;

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

// Nodes in this class are part of the IR tree purely for annotation purposes
// They have no equivalent in the wasm code generation

std::string IrWasmVisitor::visitTestNode(const std::shared_ptr<TestNode>& node) {
    if (node->getChildren().size() == 1) {
        return node->getChildren()[0]->accept(*this);      //although TEST probably shouldn't have a child
    }
    std::string nodeVar = node->getVar();
    
    //the TEST variable could be a program variable or temp variable (or value?) - return appropriate one
    if (IRSemantics::isVariable(nodeVar)) {
        return convertNumberSrcToWASM(nodeVar, getWASMNumberDatatype(nodeVar));
    }
    else{
        throw std::runtime_error("Invalid variable in visitTestNode: "+nodeVar);
    }
}

std::string IrWasmVisitor::visitEntryNode(const std::shared_ptr<EntryNode>& node) {
    // just skip the ENTRY node, and process its child
    if (node->getChildren().size() == 1) {
        return node->getChildren()[0]->accept(*this);
    }
    // in case it has no children, empty program
    return "";
}

////////////////////////////////////////////////
//////////// Other helper functions ////////////
////////////////////////////////////////////////

std::string IrWasmVisitor::convertNumberSrcToWASM(const std::string &operand, std::string expectedWasmDatatype)
{
    std::string wasmCode = "";

    //IT IS A POSITIVE INTEGER
    if (IRSemantics::isPosInteger(operand))
    {
        //just straight out print the integer constant either in i32 or i64 (Whichever one is needed)
        wasmCode += expectedWasmDatatype + ".const " + operand + "\n";
    }
    //IT IS A POSITIVE FLOAT
    else if (IRSemantics::isPosFloat(operand))
    {
        //if we are expecting a float, then just print the float constant
        if (isWASMFloatDatatype(expectedWasmDatatype))
        {
            wasmCode += expectedWasmDatatype + ".const " + operand + "\n";
        }
        //might have provided a float, but we want an integer
        //so first have the float, then truncate it to an integer
        else if (expectedWasmDatatype == "i32")
        {
            wasmCode += "f32.const " + operand + "\n";   //first print out the float constant of the same no. bytes
            wasmCode += "i32.trunc_f32_s\n";             //then truncate it to match the expected datatype
        }
        else if (expectedWasmDatatype == "i64")
        {
            wasmCode += "f32.const " + operand + "\n";
            wasmCode += "i64.trunc_f32_s\n";
        }
        else{
            throw std::runtime_error("Could not convert float "+operand+" to "+expectedWasmDatatype);
        }
    }
    //IT IS A (NON-TEMPORARY) PROGRAM VARIABLE
    else if (IRSemantics::isProgramVariable(operand)) 
    {
        wasmCode += "local.get $" + operand + "\n";
    }
    //otherwise... IT IS A TEMPORARY VARIABLE 
    //hopefully we don't need to print/get the value, it has been left on the stack
    //but we need to check if this temp variable is indeed the last one on the stack - if its not, then we must save it on to the 
    //if so, then we need to pop it off the stack
    else if (IRSemantics::isInternalTempVar(operand) && needToRestoreSwappedTemporary)
    {
        wasmCode += "local.get $tempSwap_" + getWASMNumberDatatype(operand) + "\n";
        needToRestoreSwappedTemporary = false;
    }


    //However for both user defined and temporary variables, we need to check if the datatype is correct
    //and if not, then convert it to the correct datatype
    if (IRSemantics::isVariable(operand)) {
        std::string actualWasmDatatype = variableWASMDatatypeMap[operand];  //actual datatype of variable should be in the map
        if (actualWasmDatatype != expectedWasmDatatype)
        {
            //float -> int : truncation_s
            //int -> float : conversion_s
            //i32 -> i64 : extension_s
            //i64 -> i32 : wrapping
            //f32 -> f64 : promotion
            //f64 -> f32 : demotion

            
            if (isWASMFloatDatatype(actualWasmDatatype) && isWASMIntDatatype(expectedWasmDatatype))
            {
                wasmCode += expectedWasmDatatype + ".trunc_" + actualWasmDatatype + "_s\n";
            }
            else if (isWASMIntDatatype(actualWasmDatatype) && isWASMFloatDatatype(expectedWasmDatatype))
            {
                wasmCode += expectedWasmDatatype + ".convert_" + actualWasmDatatype + "_s\n";
            }
            else if (actualWasmDatatype == "i32" && expectedWasmDatatype == "i64")
            {
                wasmCode += "i64.extend_i32_s\n";
            }
            else if (actualWasmDatatype == "i64" && expectedWasmDatatype == "i32")
            {
                wasmCode += "i32.wrap_i64\n";
            }
            else if (actualWasmDatatype == "f32" && expectedWasmDatatype == "f64")
            {
                wasmCode += "f64.promote_f32\n";
            }
            else if (actualWasmDatatype == "f64" && expectedWasmDatatype == "f32")
            {
                wasmCode += "f32.demote_f64\n";
            }
            else{
                throw std::runtime_error("Could not convert "+actualWasmDatatype+" to "+expectedWasmDatatype);
            }
        }
    }

   

    return wasmCode;
}


std::string IrWasmVisitor::convertStringSrcToWASM(const std::string &str, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies){
    std::string wasmCode = "";
    if (IRSemantics::isStringConstant(str))
    { // string constant

        unsigned long start_offset = stringMapIndicies[str][0];
        unsigned long length = stringMapIndicies[str][1];
        wasmCode += "i32.const " + std::to_string(start_offset) + "\n" +    //string still use i32 to specify the offset
                    "i32.const " + std::to_string(length) + "\n";
    }else{
        throw std::runtime_error("Operand not a string constant in convertStringSrcToWASM: " + str);
    }
    return wasmCode;
}


std::string IrWasmVisitor::convertDestToWASM(const std::string &dest) {
    std::string wasmCode = "";
    if (!IRSemantics::isInternalTempVar(dest)) {      //if program variable, then we set it
        wasmCode += "local.set $" + dest + "\n";
    }
    //otherwise it is a temp variable, so we leave it on the stack
    return wasmCode;
}

std::string IrWasmVisitor::convertDatatypeToWASM(const std::string &datatype) {
    if (datatype == "INT") {
        return "i32";
    }
    else if (datatype == "INT64") {
        return "i64";
    }
    else if (datatype == "REAL") {
        return "f32";
    }
    else if (datatype == "DOUBLE") {
        return "f64";
    }
    else {
        throw std::runtime_error("Invalid datatype in convertDatatypeToWASM: " + datatype);
    }
}

bool IrWasmVisitor::isWASMIntDatatype(const std::string& type) {
    return type == "i32" || type == "i64";
}

bool IrWasmVisitor::isWASMFloatDatatype(const std::string& type) {
    return type == "f32" || type == "f64";
}


std::string IrWasmVisitor::getWASMNumberDatatype(const std::string& item) {
    if (IRSemantics::isVariable(item)) { 
        return variableWASMDatatypeMap[item];    //find the datatype in the map
    }
    else if (IRSemantics::isPosInt32(item)) {
        return "i32";
    }
    else if (IRSemantics::isPosInt64(item)) {
        return "i64";
    }
    else if (IRSemantics::isPosFloat32(item)) {
        return "f32";
    }
    else if (IRSemantics::isPosFloat64(item)) {
        return "f64";
    }
    else{
        throw std::runtime_error("Invalid item in getWASMNumberDatatype: "+item);
    }
}


std::string IrWasmVisitor::checkAndSaveTempVarToLocal(std::string src1, std::string src2){
    //check if the first source is not a temporary variable, but the second is
    std::string wasmCode = "";
    if (!IRSemantics::isInternalTempVar(src1) && IRSemantics::isInternalTempVar(src2)){
        //if so, then we need to save the temp variable (Which is currently top of the stack) to a local variable of the correct datatype 

        needToRestoreSwappedTemporary = true;

        std::string tempDataType = getWASMNumberDatatype(src2);
        if (tempDataType == "i32") {
            addTempSwapDeclaration_i32 = true;
            wasmCode += "local.set $tempSwap_i32\n";
        }
        else if (tempDataType == "i64") {
            addTempSwapDeclaration_i64 = true;
            wasmCode += "local.set $tempSwap_i64\n";
        }
        else if (tempDataType == "f32") {
            addTempSwapDeclaration_f32 = true;
            wasmCode += "local.set $tempSwap_f32\n";
        }
        else if (tempDataType == "f64") {
            addTempSwapDeclaration_f64 = true;
            wasmCode += "local.set $tempSwap_f64\n";
        }
        else{
            throw std::runtime_error("Invalid datatype in checkAndSaveTempVarToLocal: "+tempDataType);
        }
    }
    return wasmCode;
}