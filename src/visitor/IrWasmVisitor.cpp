#include "IrWasmVisitor.h"

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

IrWasmVisitor::IrWasmVisitor(std::unordered_map<std::string, std::string> &stringMap){
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
        memoryImportCode += "(data (i32.const " + std::to_string(base_offset) + ") \"" + string_to_print + "\")" + "\n";
        stringMapIndicies[entry.first] = {base_offset, string_to_print.length()};
        base_offset += string_to_print.length();
    }
}

std::string IrWasmVisitor::getMemoryImportCode() {
    return memoryImportCode;
}

std::string IrWasmVisitor::getEntireProgramCode(const std::shared_ptr<BaseNode>& startNode) {
    
    std::string header = "(module\n";
    std::string mainCode = "(func (export \"main\")\n";
    mainCode += startNode->accept(*this);
    mainCode += ")\n)";     //to close the module and main function

    // after the main code, flags will be set, so we can add the imports
    if (importPrint) {
        header += "(import \"console\" \"log\" (func $log (param i32)))\n";
    }
    if (importPrintString) {
        header += "(import \"console\" \"logString\" (func $logString (param i32 i32)))\n";
    }
    if (importRead) {
        header += "(import \"console\" \"promptSync\" (func $read (result i32)))\n";
    }
    header += getMemoryImportCode();
    return header + mainCode;
}

std::string IrWasmVisitor::visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) {
    std::string wasmCode = convertSrcToWASM(node->getSrc1(), stringMapIndicies) + convertSrcToWASM(node->getSrc2(), stringMapIndicies);

    std::string arithOp = node->getOp();
    if (arithOp =="ADD")
        wasmCode += "i32.add\n";
    else if (arithOp == "SUB")
        wasmCode += "i32.sub\n";
    else if (arithOp == "MUL")
        wasmCode += "i32.mul\n";
    else if (arithOp == "DIV")
        wasmCode += "i32.div_s\n";    //signed division

    wasmCode += convertDestToWASM(node->getDest());

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;

}

std::string IrWasmVisitor::visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) {
    std::string wasmCode = convertSrcToWASM(node->getSrc1(), stringMapIndicies) + convertSrcToWASM(node->getSrc2(), stringMapIndicies);

    std::string logicOp = node->getOp();
    if (logicOp == "AND")
        wasmCode += "i32.and\n";
    else if (logicOp == "OR")
        wasmCode += "i32.or\n";

    wasmCode += convertDestToWASM(node->getDest());

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) {
    std::string wasmCode = convertSrcToWASM(node->getSrc(), stringMapIndicies);
    wasmCode += "i32.eqz\n";
    wasmCode += convertDestToWASM(node->getDest());

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitRelOpNode(const std::shared_ptr<RelOpNode>& node) {
    std::string wasmCode = convertSrcToWASM(node->getSrc1(), stringMapIndicies) + convertSrcToWASM(node->getSrc2(), stringMapIndicies);

    std::string relOp = node->getOp();
    if (relOp == "EQ")
        wasmCode += "i32.eq\n";
    else if (relOp == "NE")
        wasmCode += "i32.ne\n";
    else if (relOp == "LT")
        wasmCode += "i32.lt_s\n";
    else if (relOp == "GT")
        wasmCode += "i32.gt_s\n";
    else if (relOp == "LE")
        wasmCode += "i32.le_s\n";
    else if (relOp == "GE")
        wasmCode += "i32.ge_s\n";

    wasmCode += convertDestToWASM(node->getDest());

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitMovNode(const std::shared_ptr<MovNode>& node) {
    std::string wasmCode = convertSrcToWASM(node->getSrc(), stringMapIndicies);
    wasmCode += convertDestToWASM(node->getDest());
   
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
    std::string wasmCode = "(local $" + node->getVar() + " i32)\n";

    if (node->getChildren().size() == 1) {
        std::string restOfCode = node->getChildren()[0]->accept(*this);
        return wasmCode + restOfCode;
    }
    return wasmCode;
}

std::string IrWasmVisitor::visitPrintNode(const std::shared_ptr<PrintNode>& node) {
    std::string wasmCode = convertSrcToWASM(node->getSrc(), stringMapIndicies);

    if (isStringConst(node->getSrc())) {
        wasmCode += "call $logString\n";
        importPrintString = true;
    } else {
        wasmCode += "call $log\n";
        importPrint = true;
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
    
    //the TEST variable could be a program variable or temp variable - return appropriate one
    return convertSrcToWASM(node->getVar(), stringMapIndicies);
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

bool IrWasmVisitor::isPosInteger(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool IrWasmVisitor::isTempVariable(const std::string &s)
{
    return !s.empty() && s[0] == '_' && s[1] == 't';     //2 types of temp variables, _t and _s (which will show in the program)
}

bool IrWasmVisitor::isStringConst(const std::string &s)
{
    return !s.empty() && s[0] == '$';
}

std::string IrWasmVisitor::convertSrcToWASM(const std::string &operand, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies)
{
    std::string wasmCode = "";
    if (isPosInteger(operand))
    {
        wasmCode += "i32.const " + operand + "\n";
    }
    else if (isStringConst(operand))
    { // string constant

        unsigned long start_offset = stringMapIndicies[operand][0];
        unsigned long length = stringMapIndicies[operand][1];
        wasmCode += "i32.const " + std::to_string(start_offset) + "\n" +
                    "i32.const " + std::to_string(length) + "\n";
    }
    else if (!isTempVariable(operand)) // a variable name
    {
        wasmCode += "local.get $" + operand + "\n";
    }
    // otherwise its a temporary, don't print it out
    // it should have been left on top of the stack

    return wasmCode;
}

std::string IrWasmVisitor::convertDestToWASM(const std::string &dest) {
    std::string wasmCode = "";
    if (!isTempVariable(dest)) {      //if program variable, then we set it
        wasmCode += "local.set $" + dest + "\n";
    }
    //otherwise it is a temp variable, so we leave it on the stack
    return wasmCode;
}
