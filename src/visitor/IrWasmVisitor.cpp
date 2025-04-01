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
#include "DataArrayNode.h"
#include "StoreEltNode.h"
#include "LoadEltNode.h"
#include "IrTypeVisitor.h"
#include <iostream>
#include <iomanip>  //for setfill and setw

IrWasmVisitor::IrWasmVisitor(std::unordered_map<std::string, std::string> &stringMap, std::unordered_map<std::string, std::string> &irDatatypeMap){
    // constructor that converts stringMap to stringMapIndicies
    // only import memory once, and only if there is at least one string
    for (auto &entry : stringMap)
    {
        if (!requiresMemoryImport) {    //for first string, set the memory import to true
            requiresMemoryImport = true;
        }
        std::string string_to_print = entry.second;
        //base address of memory always specified as i32
        stringConstantInitialisationCode += "(data (i32.const " + std::to_string(nextAvailableMemoryOffset) + ") \"" + string_to_print + "\")" + "\n";
        stringMapIndicies[entry.first] = {nextAvailableMemoryOffset, int(string_to_print.length())};
        nextAvailableMemoryOffset += string_to_print.length();
    }

    //convert irDatatypeMap to variableWASMDatatypeMap
    for (auto &entry : irDatatypeMap) {
        variableWASMDatatypeMap[entry.first] = convertDatatypeToWASM(entry.second);
    }
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
    if (importRead_i32) {
        moduleHeader += "(import \"console\" \"promptSync\" (func $read_i32 (result i32)))\n";
    }
    if (importRead_f32) {
        moduleHeader += "(import \"console\" \"promptSync\" (func $read_f32 (result f32)))\n";
    }
    if (importRead_f64) {
        moduleHeader += "(import \"console\" \"promptSync\" (func $read_f64 (result f64)))\n";
    }
    if (importMathPow) {
        moduleHeader += "(import \"math\" \"pow\" (func $pow (param f64 f64) (result f64)))\n";   //convert operands to f64 before calling pow
    }

    if (add_tempSwapDeclaration_i32) {
        funcHeader += "(local $_tempSwap_i32 i32)\n";
    }
    if (add_tempSwapDeclaration_i64) {
        funcHeader += "(local $_tempSwap_i64 i64)\n";
    }
    if (add_tempSwapDeclaration_f32) {
        funcHeader += "(local $_tempSwap_f32 f32)\n";
    }
    if (add_tempSwapDeclaration_f64) {
        funcHeader += "(local $_tempSwap_f64 f64)\n";
    }

    std::string memoryImportCode = "";
    if (requiresMemoryImport) {
        numPagesNeeded += (estimatedExtraAllocatedBytes) / WASM_PAGE_SIZE;
        memoryImportCode = "(import \"js\" \"mem\" (memory " + std::to_string(numPagesNeeded) + "))\n";
    }

    std::string overallCode = moduleHeader + memoryImportCode + stringConstantInitialisationCode + arrayInitialisationCode + funcHeader + mainCode;
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
    if (arithOp == "SUB" || arithOp == "DIV" || arithOp == "POW"){   //non-commutative operations
        // wasmCode += checkAndSaveTempVarToLocal(nodeSrc1, nodeSrc2);   //need to check whether you need to swap operands (i.e. save then restore) on the stack
        wasmCode += checkAndSaveNonCommutativeOperands(nodeSrc1, nodeSrc2, expectedWasmDatatype);

        //then as normal, get each of the operands
        if (arithOp == "POW"){
            wasmCode += convertNumberSrcToWASM(nodeSrc1, "f64") + convertNumberSrcToWASM(nodeSrc2, "f64");     //f64 is not necessarily the expectedWasmDatatype of the result, just intermediate datatype required for Math.pow
        }
        else{
            wasmCode += convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);
        }
    } else if (arithOp == "ADD" || arithOp == "MUL"){
        //commutative operations
        //usually just process as normal src1 then src2
        wasmCode = handleCommutativeNumberOperands(nodeSrc1, nodeSrc2, expectedWasmDatatype);
    }
    //the above should handle all cases, so shouldn't need an 'else'
    

    //first check for POW - special handling
    if (arithOp == "POW"){
        if (expectedWasmDatatype == "i64"){
            throw std::runtime_error("POW operation not supported for i64");
        }
        importMathPow = true;
        wasmCode += "call $pow\n";
        
        if (expectedWasmDatatype != "f64"){    //need to convert result back to expectedWasmDatatype   
            if (isWASMIntDatatype(expectedWasmDatatype)){      //f64 -> int : truncation_s
                wasmCode += expectedWasmDatatype + ".trunc_f64_s\n";
            }else if (expectedWasmDatatype == "f32"){    //f64 -> f32 : demotion
                wasmCode += "f32.demote_f64\n";
            }else{
                throw std::runtime_error("Invalid destination in visitArithOpNode: "+nodeDest);
            }
        }
    }
    else{
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
                wasmCode += "div_s\n";   //signed division - because WASM must be able to process negative numbers (irrelevant to my IR)
            }
            else{
                wasmCode += "div\n";    
            }
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
    
    //all the operations are commutative, but may need special handling
    std::string wasmCode = handleCommutativeNumberOperands(nodeSrc1, nodeSrc2, expectedWasmDatatype);
    
    // std::string wasmCode = convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);
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

    //the destination of a relop is always i32
    //so the expectedWasmDatatype (which we do like f32.ge, even tho it will always return i32, we are free to operate on any datatype)
    //this datatype will be the larger of the two operands
    std::string largerWASMDatatype = findLargestWASMDatatype(getWASMNumberDatatype(nodeSrc1), getWASMNumberDatatype(nodeSrc2));
    expectedWasmDatatype = largerWASMDatatype;

    std::string wasmCode = "";
    if (relOp == "LT" || relOp == "GT" || relOp == "LE" || relOp == "GE"){  //non-commutative operations - check to see if we need to save and restore operands
        wasmCode += checkAndSaveNonCommutativeOperands(nodeSrc1, nodeSrc2, expectedWasmDatatype);
        wasmCode += convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);
    }else{
        //commutative operations like EQ, NE
        wasmCode += handleCommutativeNumberOperands(nodeSrc1, nodeSrc2, expectedWasmDatatype);
    }
   

    wasmCode += expectedWasmDatatype + ".";     //e.g. i32.eq
    if (relOp == "EQ")
        wasmCode += "eq\n";
    else if (relOp == "NE")
        wasmCode += "ne\n";
    else {
        if (isWASMIntDatatype(expectedWasmDatatype)) {
            if (relOp == "LT")
                wasmCode += "lt_s\n";
            else if (relOp == "GT")
                wasmCode += "gt_s\n";
            else if (relOp == "LE")
                wasmCode += "le_s\n";
            else if (relOp == "GE")
                wasmCode += "ge_s\n";
        } else {     //float data type - don't have the _s
            if (relOp == "LT")
                wasmCode += "lt\n";
            else if (relOp == "GT")
                wasmCode += "gt\n";
            else if (relOp == "LE")
                wasmCode += "le\n";
            else if (relOp == "GE")
                wasmCode += "ge\n";
        }
    }

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

    if (node->getText() == "ENDLOOP"){
        exitStack.pop();
    }

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitExitNode(const std::shared_ptr<ExitNode>& node) {
    std::string endloopLabel = exitStack.top();     //should pop it off when we process the ENDLOOP node

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
    exitStack.push(node->getEndloopLabel());    //should pop it off when we process the ENDLOOP node
    wasmCode += node->getChildren()[0]->accept(*this);    // process the body of the loop
    wasmCode += "br $" + node->getBodyLabel() + "\n)\n";     // for end of one of the block statement
    
    std::string endloopCode = node->getChildren()[1]->accept(*this);
    return wasmCode + endloopCode;
}

std::string IrWasmVisitor::visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) {
    std::string wasmCode = "(block $" + node->getEndLoopLabel() + "\n";
    
    //sandwich the initialisation code between the block and the loop - unnecessary but might make it clearer it is the initialisation logic
    std::string initCode = node->getInitNode()->accept(*this);       
    wasmCode += initCode;

    //add the endloopLabel to the stack - even if no exit in the loop, we need to pop it off when we finish processing the loop
    exitStack.push(node->getEndLoopLabel());

    wasmCode += "(loop $" + node->getBodyLabel() + "\n";
    std::string conditionCode = node->getCondNode()->accept(*this);
    //does not need to use exitStack here, since we know where the EXIT is 
    //we are adding it at the start of the loop, after the termination condition check
    std::string extraTerminationCode = "(if \n(then \n br $" + node->getEndLoopLabel() + "\n)\n)\n";
    std::string bodyCode = node->getBodyNode()->accept(*this);
    std::string stepCode = node->getStepNode()->accept(*this);
    std::string extraEndloopCode = "br $" + node->getBodyLabel() + "\n)\n";
    std::string afterEndNodeCode = node->getEndLoopNode()->accept(*this);

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
    std::string nodeVar = node->getVar();    
    std::string wasmDatatype = variableWASMDatatypeMap[nodeVar];   //get the wasm datatype of the variable
    
    std::string wasmCode = "(local $" + nodeVar + " " + wasmDatatype + ")\n";
    
    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) {
    requiresMemoryImport = true;      //having arrays means we need to import memory from JS

    std::string arrayVar = node->getArrayVar();    
    std::string wasmDatatype = variableWASMDatatypeMap[arrayVar];   //get the wasm datatype of the variable
    
    //when declaring an array, need to reserve some memory of the correct size for the array (even if we don't put any data there)
    //so have a map that stores the base index of the array, and also the size of the array (which we can get from the array type and dimensions)
    
    std::vector<int> arrayDimensions = node->getArrayDimensions();
    int arrayEltSize = 1;
    for (int dim : arrayDimensions) {
        arrayEltSize *= dim;
    }

    int arrayByteSize = arrayEltSize * getWASMByteSize(wasmDatatype);
    estimatedExtraAllocatedBytes += arrayByteSize;

    arrayVariablesMemoryMap[arrayVar] = {nextAvailableMemoryOffset, arrayDimensions};
    nextAvailableMemoryOffset += arrayByteSize;
    
    //get the rest of the code
    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return restOfCode;
    }
    return "";
}

std::string IrWasmVisitor::visitDataArrayNode(const std::shared_ptr<DataArrayNode>& node) {
    //byte-addressed array initialisation
    //e.g (data (i32.const <base address>) "\01\00\00\00\02\00\00\00\03\00\00\00")  ;; Initialize memory with [1, 2, 3]
    //each 2 hex digits is 1 byte

    //the offset is always specified as i32 (how you interpret the data is a different matter)
    std::string arrayVar = node->getArrayVar();
    std::string arrayBaseAddress = std::to_string(arrayVariablesMemoryMap[arrayVar].first);
    std::string wasmCode = "(data (i32.const " + arrayBaseAddress + ") ";
   
    std::string wasmDatatype = variableWASMDatatypeMap[arrayVar];
    std::vector<std::string> dataValues = node->getDataValues();

    wasmCode += "\"";
    for (std::string value : dataValues) {
        wasmCode += getHexByteInitialisationString(value, wasmDatatype);
    }
    wasmCode += "\")\n";

    arrayInitialisationCode += wasmCode;    //add the initialisation code for the array

    //return the rest of the code
    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return restOfCode;
    }
    return "";
}

std::string IrWasmVisitor::visitPrintNode(const std::shared_ptr<PrintNode>& node) {
    std::string nodeSrc = node->getSrc();
    std::string wasmCode = "";
    if (arrayVariablesMemoryMap.find(nodeSrc) != arrayVariablesMemoryMap.end()) {   //print an array
        std::string arrayBaseAddress = std::to_string(arrayVariablesMemoryMap[nodeSrc].first);
        std::string arrayDatatype = variableWASMDatatypeMap[nodeSrc];
        std::vector<int> arrayDimensions = arrayVariablesMemoryMap[nodeSrc].second;
        int arraySize = 1;
        for (int dim : arrayDimensions) {
            arraySize *= dim;
        }
        for (int i=0; i<arraySize; i++) {
            int indexToPrintAt = std::stoi(arrayBaseAddress) + i*getWASMByteSize(arrayDatatype);
            wasmCode += "i32.const " + std::to_string(indexToPrintAt) + "\n";   //specify the index to load from
            wasmCode += arrayDatatype + ".load\n";

            if (arrayDatatype == "i32"){
                wasmCode += "call $log_i32\n";
                importPrint_i32 = true;
            }
            else if (arrayDatatype == "i64"){
                wasmCode += "call $log_i64\n";
                importPrint_i64 = true;
            }
            else if (arrayDatatype == "f32"){
                wasmCode += "call $log_f32\n";
                importPrint_f32 = true;
            }
            else if (arrayDatatype == "f64"){
                wasmCode += "call $log_f64\n";
                importPrint_f64 = true;
            }
            else{
                throw std::runtime_error("Invalid datatype in visitPrintNode: "+arrayDatatype);
            }
        }
    }else {
        if (IRSemantics::isStringConstant(nodeSrc)) {
            wasmCode += convertStringSrcToWASM(nodeSrc, stringMapIndicies);
        }
        else{ //its a number (const or variable)
            wasmCode += convertNumberSrcToWASM(nodeSrc, getWASMNumberDatatype(nodeSrc));    //??? need to check src datatype
        }

        ///TODO: handle different datatypes for printing - maybe for arrays too??!
        if (IRSemantics::isStringConstant(nodeSrc)) {
            wasmCode += "call $logString\n";
            importPrintString = true;
        } 
        else {
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
    }

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitReadNode(const std::shared_ptr<ReadNode>& node) {
    std::string expectedWasmDatatype = getWASMNumberDatatype(node->getVar());
    std::string wasmCode = "call $read_" + expectedWasmDatatype + "\n";
    wasmCode += "local.set $" + node->getVar() + "\n";
    if (expectedWasmDatatype == "i32"){
        importRead_i32 = true;
    }
    else if (expectedWasmDatatype == "f32"){
        importRead_f32 = true;
    }
    else if (expectedWasmDatatype == "f64"){
        importRead_f64 = true;
    }
    else{
        throw std::runtime_error("Invalid datatype in visitReadNode: "+expectedWasmDatatype);
    }

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
    if (!IRSemantics::isStringConstant(nodeVar)) {
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

std::string IrWasmVisitor::visitStoreEltNode(const std::shared_ptr<StoreEltNode>& node) {
    std::string wasmCode = "";
    std::string arrayVar = node->getArrayVar();
    std::vector<std::string> indices = node->getIndicies();
    std::string valueToStore = node->getSrc();

    //value to store may be a temp variable - so we need to save it to restore later in the correct order
    ///NOTE:since STORE_ELT is a non-commutative operation, the order of the operands is important
   
    //for store instructions, need to save the top of the stack to a local variable in two cases:
    //1. <non-temp> <temp>  -  since its non-commutative, need to get src1 first
    //2. <temp index> <temp value>   - since we also need to add code to the index calculation
    ///HENCE: we need to save top of stack whenever the valueToStore is a temp variable

    if (IRSemantics::isInternalTempVar(valueToStore)){
        wasmCode += saveTempVarToLocal(valueToStore);
    }
        

    //get the wasm datatype of the array
    std::string arrayWasmDatatype = variableWASMDatatypeMap[arrayVar];      //type of the array - take that as the expected datatype
    // std::string valueWasmDatatype = getWASMNumberDatatype(valueToStore);    //type of the value we are trying store

   
    //get the offset of the array element to store to - need array dimensions
    std::vector<int> arrayDimensions = arrayVariablesMemoryMap[arrayVar].second;
    wasmCode += convertIndexOffsetToWASM(indices, arrayDimensions, arrayWasmDatatype);  //would either compute at runtime or compile time the index

    //add the base address to the offset in the wasm
    int baseAddress = arrayVariablesMemoryMap[arrayVar].first;
    wasmCode += "i32.const " + std::to_string(baseAddress) + "\n";
    wasmCode += "i32.add\n";

    //put the value to store on the stack - might have already been computed - so need to restore from temp swap var
    wasmCode += convertNumberSrcToWASM(valueToStore, arrayWasmDatatype);

    //<datatype>.store
    wasmCode += arrayWasmDatatype + ".store\n";

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitLoadEltNode(const std::shared_ptr<LoadEltNode>& node) {
    std::string wasmCode = "";
    std::string arrayVar = node->getArrayVar();
    std::vector<std::string> indices = node->getIndicies();
    std::string dest = node->getDest();

    //get the wasm datatype of the array
    std::string arrayWasmDatatype = variableWASMDatatypeMap[arrayVar];      //type of the array - take that as the expected datatype

    //get the offset of the array element to store to - need array dimensions
    std::vector<int> arrayDimensions = arrayVariablesMemoryMap[arrayVar].second;
    wasmCode += convertIndexOffsetToWASM(indices, arrayDimensions, arrayWasmDatatype);  //would either compute at runtime or compile time the index

    //add the base address to the offset in the wasm
    int baseAddress = arrayVariablesMemoryMap[arrayVar].first;
    wasmCode += "i32.const " + std::to_string(baseAddress) + "\n";
    wasmCode += "i32.add\n";

    //put the value to store on the stack
    wasmCode += arrayWasmDatatype + ".load\n";

    //put the value in the destination variable
    wasmCode += convertDestToWASM(dest);

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;

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
            if (IRSemantics::isPosFloat32(operand)) {
                wasmCode += "f32.const " + operand + "\n";   //first print out the float constant of the same no. bytes
                wasmCode += "i32.trunc_f32_s\n";             //then truncate it to match the expected datatype
            } else if (IRSemantics::isPosFloat64(operand)) {
                wasmCode += "f64.const " + operand + "\n";   
                wasmCode += "i32.trunc_f64_s\n";             
            }
        }
        else if (expectedWasmDatatype == "i64")
        {
            if (IRSemantics::isPosFloat32(operand)) {
                wasmCode += "f32.const " + operand + "\n";
                wasmCode += "i64.trunc_f32_s\n";
            } else if (IRSemantics::isPosFloat64(operand)) {
                wasmCode += "f64.const " + operand + "\n";
                wasmCode += "i64.trunc_f64_s\n";
            }
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
    //but it might have been saved to a temporary local variable (operands needed to be swapped) - so we should pop and restore it now
    else if (IRSemantics::isInternalTempVar(operand) && tempSwapVariableName == operand)
    {
        wasmCode += "local.get $_tempSwap_" + getWASMNumberDatatype(operand) + "\n";
        tempSwapVariableName = "";
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


std::string IrWasmVisitor::convertStringSrcToWASM(const std::string &str, std::unordered_map<std::string, std::array<int, 2>> &stringMapIndicies){
    std::string wasmCode = "";
    if (IRSemantics::isStringConstant(str))
    { // string constant

        int start_offset = stringMapIndicies[str][0];
        int length = stringMapIndicies[str][1];
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

std::string IrWasmVisitor::findLargestWASMDatatype(const std::string& type1, const std::string& type2){
    std::vector<std::string> highestPriority = {"f64", "f32", "i64", "i32"};
    for (int i=0; i<highestPriority.size() ; i++){
        if (type1 == highestPriority[i] || type2 == highestPriority[i]){
            return highestPriority[i];    //stop and return the highest priority datatype
        }
    }
    throw std::runtime_error("Could not find largest WASM datatype between "+type1+" and "+type2);
}

std::string IrWasmVisitor::saveTempVarToLocal(std::string tempVar){
    std::string wasmCode = "";
    // needToRestoreSwappedTemporary = true;
    tempSwapVariableName = tempVar;
    std::string tempVarType = getWASMNumberDatatype(tempVar);
    if (tempVarType == "i32") {
        add_tempSwapDeclaration_i32 = true;
        wasmCode += "local.set $_tempSwap_i32\n";
    }
    else if (tempVarType == "i64") {
        add_tempSwapDeclaration_i64 = true;
        wasmCode += "local.set $_tempSwap_i64\n";
    }
    else if (tempVarType == "f32") {
        add_tempSwapDeclaration_f32 = true;
        wasmCode += "local.set $_tempSwap_f32\n";
    }
    else if (tempVarType == "f64") {
        add_tempSwapDeclaration_f64 = true;
        wasmCode += "local.set $_tempSwap_f64\n";
    }
    else{
        throw std::runtime_error("Invalid datatype in saveTempVarToLocal: "+tempVarType);
    }
    return wasmCode;
}

std::string IrWasmVisitor::handleCommutativeNumberOperands(std::string nodeSrc1, std::string nodeSrc2, std::string expectedWasmDatatype){
    std::string wasmCode = "";
    //all the operations are commutative, but may need special handling
    

    if (!IRSemantics::isInternalTempVar(nodeSrc1) && IRSemantics::isInternalTempVar(nodeSrc2)){    
    //CASE1: if we have <non-temp> <temp>, then we process in reverse order src2 then src1 (since src2 was already on the stack)
        wasmCode += convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype);
    }
    else if (IRSemantics::isInternalTempVar(nodeSrc1) && IRSemantics::isInternalTempVar(nodeSrc2)){
        //CASE 2: OP <temp> <temp>
        //this case is usually fine, unless you need to add something to src1 (which is on the bottom of the stack)
        //in which case we need to save src2 to a local variable, to operate on src1 first
        if (variableWASMDatatypeMap[nodeSrc1] != expectedWasmDatatype){
            wasmCode += saveTempVarToLocal(nodeSrc2);
        }
        wasmCode += convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);
    }
    else{    //normal order
        wasmCode += convertNumberSrcToWASM(nodeSrc1, expectedWasmDatatype) + convertNumberSrcToWASM(nodeSrc2, expectedWasmDatatype);
    }

    return wasmCode;
}

std::string IrWasmVisitor::checkAndSaveNonCommutativeOperands(std::string nodeSrc1, std::string nodeSrc2, std::string expectedWasmDatatype){
    std::string wasmCode = "";
    if (!IRSemantics::isInternalTempVar(nodeSrc1) && IRSemantics::isInternalTempVar(nodeSrc2)){
        //CASE 1: OP <non-temp> <temp>
        //nodeSrc2 is at bottom of the stack, but we need to swap the order, since operation is non-commutative
        wasmCode += saveTempVarToLocal(nodeSrc2);
    }
    else if (IRSemantics::isInternalTempVar(nodeSrc1) && IRSemantics::isInternalTempVar(nodeSrc2)){
        //CASE 2: OP <temp> <temp>
        //this case is usually fine, unless you need to add something to src1 (which is on the bottom of the stack)
            //in which case we need to save src2 to a local variable, to operate on src1 first
        if (variableWASMDatatypeMap[nodeSrc1] != expectedWasmDatatype){
        wasmCode += saveTempVarToLocal(nodeSrc2);
    }
    }
    return wasmCode;
}


int IrWasmVisitor::getWASMByteSize(const std::string& type) {
    if (type == "i32" || type == "f32") {
        return 4;
    }
    else if (type == "i64" || type == "f64") {
        return 8;
    }
    else{
        throw std::runtime_error("Invalid datatype in getWASMByteSize: "+type);
    }
}

std::string IrWasmVisitor::getHexByteInitialisationString(const std::string& number, const std::string& numtype){
    //e.g. returns "\01\00\00\00" for 1 as a i32
    if (numtype == "i32"){
            int32_t num = std::stoi(number);
            std::stringstream ss;
            ss << std::setw(8) << std::setfill('0') << std::hex << static_cast<uint32_t>(num);
            std::string hexString = ss.str();
            //current format is 00000001

            //turn into little endian and add backslashes
            std::string littleEndianHexString = "";
            for (int i = 6; i >= 0; i-=2) {
                littleEndianHexString += "\\";
                littleEndianHexString += hexString.substr(i, 2);
            }

            return littleEndianHexString;
    }
    else if (numtype == "i64"){
        int64_t num = std::stoull(number);
        std::stringstream ss;
        ss << std::setw(16) << std::setfill('0') << std::hex << num;
        std::string hexString = ss.str();
        //current format is 0000000000000001

        //turn into little endian and add backslashes
        std::string littleEndianHexString = "";
        for (int i = 14; i >= 0; i-=2) {
            littleEndianHexString += "\\";
            littleEndianHexString += hexString.substr(i, 2);
        }

        return littleEndianHexString;
    }
    else if (numtype == "f32"){
        float num = std::stof(number);
        uint32_t int_rep;
        std::memcpy(&int_rep, &num, sizeof(float));  // Reinterpret float as uint32_t, since std::hex only works for ints
        return getHexByteInitialisationString(std::to_string(int_rep), "i32");
    }
    else if (numtype == "f64"){
        double num = std::stod(number);
        uint64_t int_rep;
        std::memcpy(&int_rep, &num, sizeof(double));  // Reinterpret double as uint64_t, since std::hex only works for ints
        return getHexByteInitialisationString(std::to_string(int_rep), "i64");
    }
    else{
        throw std::runtime_error("Invalid datatype in getHexByteInitialisationString: "+numtype);
    }
}

std::string IrWasmVisitor::convertIndexOffsetToWASM(const std::vector<std::string>& indices, const std::vector<int>& dimensions, const std::string& arrayWasmDatatype){
    std::string wasmCode = "";
    for (int d = indices.size() - 1; d >= 0; d--) {
        //the datatype of the indicies should always be i32

        wasmCode += convertNumberSrcToWASM(indices[d], "i32");     //the index could be a variable
        wasmCode += "i32.const 1\n";  //need to subtract 1 for index calculation since it is 1-based
        wasmCode += "i32.sub\n";   

        //calculate the dimension product from 1 to d-1 statically in the compiler
        //then do final multiplication at runtime
        int dimensionProduct = 1;
        for (int i=0; i<d; i++){
            dimensionProduct *= dimensions[i];
        }
        if (dimensionProduct != 1){
            wasmCode += "i32.const " + std::to_string(dimensionProduct) + "\n";
            wasmCode += "i32.mul\n";
        }

        //if not the last index, then add the result to the next index
        if (d<indices.size()-1){
            wasmCode += "i32.add\n";
        }
    }
    //integer index has been calculated, but need to multiply by the byte size of the datatype
    wasmCode += "i32.const " + std::to_string(getWASMByteSize(arrayWasmDatatype)) + "\n";
    wasmCode += "i32.mul\n";

    return wasmCode;
}