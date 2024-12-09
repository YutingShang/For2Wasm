#include "IRTreeTools.h"

std::string IRTreeTools::irTreeToDotTree(BaseNode *root)
{

    std::stringstream dot;
    dot << "digraph Tree {\n";

    int nodeCount = 0;
    std::unordered_map<BaseNode *, int> nodeToIndex; // map to store node and its index - used in dot file

    std::stack<BaseNode *> stack;
    stack.push(root);

    while (!stack.empty())
    {
        BaseNode *currentNode = stack.top();
        stack.pop();

        if (nodeToIndex.find(currentNode) == nodeToIndex.end())
        {
            nodeToIndex[currentNode] = nodeCount++; // add node to map if not already in it

            // get text representation of node
            std::string nodeText = currentNode->getText();
            nodeText = std::regex_replace(nodeText, std::regex("\""), "\\\""); // escape quotes for printing
            dot << "node" << nodeToIndex[currentNode] << " [label=\"" << nodeText << "\"];\n";
        }

        // add children to stack
        for (BaseNode *child : currentNode->getChildren())
        {
            // create child node if not in map
            if (nodeToIndex.find(child) == nodeToIndex.end())
            {
                nodeToIndex[child] = nodeCount++;
                std::string childText = child->getText();
                childText = std::regex_replace(childText, std::regex("\""), "\\\"");
                dot << "node" << nodeToIndex[child] << " [label=\"" << childText << "\"];\n";
            }

            // add edge to dot file
            dot << "node" << nodeToIndex[currentNode] << " -> node" << nodeToIndex[child] << ";\n";

            // add child to stack
            stack.push(child);
        }
    }

    dot << "}\n";
    return dot.str();
}

std::string IRTreeTools::irTreeToWASM(BaseNode *root, std::unordered_map<std::string, std::string> &stringMap)
{

    /// TODO:need to maintain all the states, strings, temp variables, and imports, and then cases

    // start with the module
    std::string headerCode = "(module\n";

    std::vector<std::string> exitStack;
    std::unordered_map<std::string, std::array<unsigned long, 2>> stringMapIndicies;

    // convert stringMap to stringMapIndicies
    unsigned long base_offset = 0;
    bool importedMemory = false; // only import memory once, and only if there is at least one string

    /// NOTE: I am importing the memory first (data section)
    // Before the import statements for operations such as logging and reading (this is added after parsing the tree)
    for (auto &entry : stringMap)
    {
        if (!importedMemory)
        {
            headerCode += "(import \"js\" \"mem\" (memory 1))\n";
            importedMemory = true;
        }
        std::string string_to_print = entry.second;
        headerCode += "(data (i32.const " + std::to_string(base_offset) + ") \"" + string_to_print + "\")" + "\n";
        stringMapIndicies[entry.first] = {base_offset, string_to_print.length()};
        base_offset += string_to_print.length();
    }

    // traverse the IR tree, DFS preorder left to right traversal, and convert it to WASM code
    std::string wasmCode = "";
    std::vector<BaseNode *> nodesToVisit = {root};

    processToWASM(root, stringMapIndicies, exitStack, wasmCode);

    // add imports to the header after traversing the tree, so you know which ones are needed
    if (importPrintString)
    {
        headerCode += "(import \"console\" \"logString\" (func $logString (param i32 i32)))\n";
    }
    if (importPrint)
    {
        headerCode += "(import \"console\" \"log\" (func $log (param i32)))\n";
    }
    if (importRead)
    {
        headerCode += "(import \"console\" \"promptSync\" (func $read (result i32)))\n";
    }

    // start the main function
    headerCode += "(func (export \"main\")\n";

    std::string finalWASM = headerCode + wasmCode + ")\n)";

    return finalWASM;
}

bool IRTreeTools::isPosInteger(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool IRTreeTools::isTempVariable(const std::string &s)
{
    return !s.empty() && s[0] == '_';
}

bool IRTreeTools::isStringConst(const std::string &s)
{
    return !s.empty() && s[0] == '$';
}

std::string IRTreeTools::convertOperandToWASM(const std::string &operand, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies)
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

    return wasmCode;
}

void IRTreeTools::visitWASM(BaseNode *nodeToProcess, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies, std::vector<std::string> &exitStack, std::string &outputString)
{
    outputString += nodeToProcess->getText() + "\n";
    nodeToProcess->setVisited(true);

    for (BaseNode *child : nodeToProcess->getChildren())
    {
        if (!child->getVisited())
        {
            visitWASM(child, stringMapIndicies, exitStack, outputString);
        }
    }
}

void IRTreeTools::processToWASM(BaseNode *nodeToProcess, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMapIndicies, std::vector<std::string> &exitStack, std::string &wasmCode)
{
    // maybe put the stringMap and exitStack in the class attribute
    // maybe add a visited to the node class

    std::vector<std::string> instruction = nodeToProcess->getTextVector();
    nodeToProcess->setVisited(true);

    // check if the code requires any import statements
    if (instruction[0] == "CALL" && instruction[1] == "PRINT" && isStringConst(instruction[2]))
    {
        importPrintString = true;
    }
    else if (instruction[0] == "CALL" && instruction[1] == "PRINT" && !isStringConst(instruction[2]))
    { // if didn't match first case, then just a normal print, not print string
        importPrint = true;
    }
    else if (instruction[0] == "CALL" && instruction[1] == "READ")
    {
        importRead = true;
    }

    /// NOTE: each string will add its own \n to the end of the string

    if (instruction[0] == "DECLARE")
    {
        wasmCode += "(local $" + instruction[1] + " i32)\n";
    }
    else if (instruction[0] == "MOV")
    {
        std::string src = instruction[2];
        std::string dest = instruction[1];
        wasmCode += convertOperandToWASM(src, stringMapIndicies) + "local.set $" + dest + "\n";
    }
    else if (instruction[0] == "ADD")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.add\n";
    }
    else if (instruction[0] == "SUB")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.sub\n";
    }
    else if (instruction[0] == "MUL")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.mul\n";
    }
    else if (instruction[0] == "DIV")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.div_s\n";
    }
    else if (instruction[0] == "EQ")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.eq\n";
    }
    else if (instruction[0] == "NE")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.ne\n";
    }
    else if (instruction[0] == "LT")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.lt_s\n";
    }
    else if (instruction[0] == "GT")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.gt_s\n";
    }
    else if (instruction[0] == "LE")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.le_s\n";
    }
    else if (instruction[0] == "GE")
    {
        std::string src1 = instruction[2];
        std::string src2 = instruction[3];
        wasmCode += convertOperandToWASM(src1, stringMapIndicies) + convertOperandToWASM(src2, stringMapIndicies) + "i32.ge_s\n";
    }

    else if (instruction[0] == "IF")
    {

        // process condition - go until "TEST <var>"
        processToWASM(nodeToProcess->getChildren()[0], stringMapIndicies, exitStack, wasmCode);
        wasmCode += "(if \n";

        // process the then block, go until "ENDTHEN"
        wasmCode += "(then\n";
        processToWASM(nodeToProcess->getChildren()[1], stringMapIndicies, exitStack, wasmCode);

        // process the else block if it exists, go until "ENDELSE"
        if (nodeToProcess->getChildren().size() == 4)
        {
            wasmCode += "(else\n";
            processToWASM(nodeToProcess->getChildren()[2], stringMapIndicies, exitStack, wasmCode);
            processToWASM(nodeToProcess->getChildren()[3], stringMapIndicies, exitStack, wasmCode); // process the ENDIF
        }
        else
        { // process the ENDIF if no else block
            processToWASM(nodeToProcess->getChildren()[2], stringMapIndicies, exitStack, wasmCode);
        }
    }
    else if (instruction[0] == "ENDTHEN" || instruction[0] == "ENDELSE" || instruction[0] == "ENDIF" || instruction[0] == "ENDLOOP")
    {
        if (instruction[0] == "ENDIF")
        {
            wasmCode += ")\n";
        }
        else if (instruction[0] == "ENDELSE")
        {
            wasmCode += ")\n";
        }
        else if (instruction[0] == "ENDLOOP")
        {
            wasmCode += ")\n";
        }
        else
        {
            wasmCode += ")\n";
        }
    }
    else if (instruction[0] == "CALL" && instruction[1] == "PRINT")
    {
        std::string src = instruction[2];
        wasmCode += convertOperandToWASM(src, stringMapIndicies);
        wasmCode += isStringConst(src) ? "call $logString\n" : "call $log\n";
    }
    else if (instruction[0] == "CALL" && instruction[1] == "READ")
    {
        wasmCode += "call $read\n";
        wasmCode += "local.set $" + instruction[2] + "\n";
    }
    else if (instruction[0] == "LOOP")
    {
        std::string bodyLabel = instruction[1];
        std::string exitLabel = instruction[2];

        wasmCode += "(block $" + exitLabel + "\n";
        wasmCode += "(loop $" + bodyLabel + "\n";

        exitStack.push_back(exitLabel);

        processToWASM(nodeToProcess->getChildren()[0], stringMapIndicies, exitStack, wasmCode);

        // add the end of loop break statement
        wasmCode += "br $" + bodyLabel + "\n)\n";
    }
    else if (instruction[0] == "EXIT")
    {
        std::string exitLabel = exitStack.back();
        exitStack.pop_back(); /// QUESTION: do I need to pop back??

        wasmCode += "br $" + exitLabel + "\n";
    }
    else if (instruction[0] == "TEST" || instruction[0] == "ENTRY")
    {
        // ignore these instructions
    }
    else
    { // not been processed - print out the IR instruction
        wasmCode += nodeToProcess->getText() + "\n";
    }

    // process children - DFS
    for (BaseNode *child : nodeToProcess->getChildren())
    {
        if (!child->getVisited())
        {
            processToWASM(child, stringMapIndicies, exitStack, wasmCode);
        }
    }
}