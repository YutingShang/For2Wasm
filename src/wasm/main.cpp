#include <iostream>
#include <fstream>
#include <sstream>

void printOperand(const std::string &operand);

/// NOTE: this will return the next line number to process
int processLine(const std::vector<std::vector<std::string>> &lines, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMap, std::vector<std::string> &exitStack, const int lineNumber, bool stripLabel);

int main(int, const char **)
{
    // open the IR file
    std::ifstream inFile("../output/output.ir");

    if (!inFile.is_open())
    {
        std::cerr << "Failed to open input IR file" << std::endl;
        return 1;
    }

    // process each line of the file into 2D array of tokens
    std::vector<std::vector<std::string>> lines;
    for (std::string line; std::getline(inFile, line);)
    {
        std::vector<std::string> lineTokens;
        std::istringstream stream(line);
        std::string token;

        while (stream >> token)
        { // split the line into tokens, removing whitespaces
            lineTokens.push_back(token);
        }

        lines.push_back(lineTokens);
    }

    //////////////////////// first pass to make any imports//////////////

    //start the module
    std::cout << "(module" << std::endl;

    bool importPrint = false;
    bool importPrintString = false;
    bool importRead = false;

    for (auto line : lines)
    {
        int i = 0;
        while (i < line.size())
        {
            if (line[i] == "CALL" && line[i + 1] == "PRINT" && line[i + 2][0] != '$')
            {
                importPrint = true;
            }
            else if (line[i] == "CALL" && line[i + 1] == "PRINT" && line[i + 2][0] == '$')
            {
                importPrintString = true;
            }
            else if (line[i] == "CALL" && line[i + 1] == "READ")
            {
                importRead = true;
            }
            i++;
        }
    }

    if (importPrint)
    {
        std::cout << "(import \"console\" \"log\" (func $log (param i32)))" << std::endl;
    }
    if (importPrintString)
    {
        std::cout << "(import \"console\" \"logString\" (func $logString (param i32 i32)))" << std::endl;
    }
    if (importRead)
    {
        std::cout << "(import \"console\" \"promptSync\" (func $read (result i32)))" << std::endl;
    }

    /////////////////// retrieve strings from the translator visitor

    std::ifstream stringFile("../output/index_strings.txt");
    if (!stringFile.is_open())
    {
        std::cerr << "Failed to open string file" << std::endl;
        return 1;
    }

    // read the strings into a map of <str_var, [start_offset, length]>
    // e.g. if $str2 = "Add:"
    // then stringMap["$str1"] = [50,4]
    std::unordered_map<std::string, std::array<unsigned long, 2>> stringMap;
    unsigned long base_offset = 0;
    bool importedMemory = false;

    for (std::string index; std::getline(stringFile, index);) // iterate through every line of the file, first the <str_var>, then the next line containing the actual string
    {
        if (!importedMemory){      //if at least a string exists, import 1 page of memory
            std::cout << "(import \"js\" \"mem\" (memory 1))" << std::endl;
            importedMemory = true;
        }

        std::string string_to_print;
        std::getline(stringFile, string_to_print);                                                          // get the actual string to print
        std::cout << "(data (i32.const " << base_offset << ") \"" << string_to_print << "\")" << std::endl; // print the string to the data section

        stringMap[index] = {base_offset, string_to_print.length()}; // store the start offset and length of the string
        base_offset += string_to_print.length();
    }

    ///////////////////// second pass to process the lines

    // start the main function 
    std::cout << "(func (export \"main\")" << std::endl;

    int lineNumber = 0;
    std::vector<std::string> exitStack;
    while (lineNumber < lines.size())
    {
        lineNumber = processLine(lines, stringMap, exitStack, lineNumber, false); // will get the next line number to process
    }

    // end the main function
    std::cout << ")\n)" << std::endl;

    /////////////write the output to a file////////////////////////

    return 0;
}

bool isPosInteger(const std::string &s)
{
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

bool isTempVariable(const std::string &s)
{
    return !s.empty() && s[0] == '_';
}

bool isStringConst(const std::string &s)
{
    return !s.empty() && s[0] == '$';
}

void printOperand(const std::string &operand)
{
    if (isPosInteger(operand))
    {
        std::cout << "i32.const " << operand << "\n";
    }
    else if (!isTempVariable(operand))
    { // a variable name
        std::cout << "local.get $" << operand << "\n";
    } // otherwise its a temporary, don't print it out

    /// TODO: check if its a string constant
}

bool isCondLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0, 4) == "cond";
}

bool isThenLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0, 4) == "then";
}

bool isElseLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0, 4) == "else";
}

bool isEndIfLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0, 5) == "endif";
}

bool isLoopBodyLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0, 4) == "body";
}

bool isLoopExitLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0, 4) == "exit";
}

bool isLabel(const std::string &s)
{
    return isCondLabel(s) || isThenLabel(s) || isElseLabel(s) || isEndIfLabel(s) || isLoopBodyLabel(s) || isLoopExitLabel(s); // add other labels too
}

int processLine(const std::vector<std::vector<std::string>> &lines, std::unordered_map<std::string, std::array<unsigned long, 2>> &stringMap, std::vector<std::string> &exitStack, const int lineNumber, bool stripLabel)
{
    std::vector<std::string> line = lines[lineNumber];
    if (stripLabel && isLabel(line[0]))
    {
        line.erase(line.begin()); // remove the label
    }

    if (line[0] == "DECLARE")
    {
        std::cout << "(local $" << line[1] << " i32)" << std::endl;
    }
    else if (line[0] == "MOV")
    {
        std::string dest = line[1];
        // std::cout <<"(local.set $"<<dest<<" ";

        std::string src = line[2];
        // check if src is a number or a variable
        //  if (isPosInteger(src)){
        //      std::cout << "(i32.const " << src << "))" << std::endl;
        //  }else{
        //      std::cout << "(local.get $"<<src<<"))" << std::endl;
        //  }
        printOperand(src);
        std::cout << "local.set $" << dest << std::endl;
    }
    else if (line[0] == "ADD" || line[0] == "SUB" || line[0] == "MUL" || line[0] == "DIV")
    {
        // in my AST, ADD will always be ADD <tmp_dest> <src1> <src2>
        // so in wasm, the temp_dest will just be implicit on the stack
        std::string src1 = line[2];
        std::string src2 = line[3];

        printOperand(src1);
        printOperand(src2);

        if (line[0] == "ADD")
        {
            std::cout << "i32.add" << std::endl;
        }
        else if (line[0] == "SUB")
        {
            std::cout << "i32.sub" << std::endl;
        }
        else if (line[0] == "MUL")
        {
            std::cout << "i32.mul" << std::endl;
        }
        else if (line[0] == "DIV")
        {
            std::cout << "i32.div_s" << std::endl;    //signed division
        }else{
            std::cerr << "Invalid operation" << std::endl;
        }
        
    }
    else if (line[0] == "IF")
    {
        // process condition - go until "TEST <var>"

        // process the then block, go until "ENDTHEN"

        // process the else block if it exists, go until "ENDELSE"

        // print nothing when we actually see this "IF"
    }

    else if (isCondLabel(line[0]))
    {
        processLine(lines, stringMap, exitStack, lineNumber, true); // process the same line, strip the cond label
    }
    else if (line[0] == "TEST")
    {

        /// NOTE: this is assuming cond labels are only used for 'IF' statements
        std::cout << "(if" << std::endl;
    }
    else if (line[0] == "EQ" || line[0] == "NE" || line[0] == "LT" || line[0] == "GT" || line[0] == "LE" || line[0] == "GE")
    {
        // result will be saved to a temp var - implicit on the stack
        std::string src1 = line[2];
        std::string src2 = line[3];

        printOperand(src1);
        printOperand(src2);

        if (line[0] == "EQ")
            std::cout << "i32.eq" << std::endl;
        else if (line[0] == "NE")
            std::cout << "i32.ne" << std::endl;
        else if (line[0] == "LT")
            std::cout << "i32.lt_s" << std::endl; //signed less than
        else if (line[0] == "GT")
            std::cout << "i32.gt_s" << std::endl; //signed greater than
        else if (line[0] == "LE")
            std::cout << "i32.le_s" << std::endl; //signed less than or equal
        else if (line[0] == "GE")
            std::cout << "i32.ge_s" << std::endl; //signed greater than or equal
        else{
            std::cerr << "Invalid relational operation" << std::endl;
        }
    }
    else if (isThenLabel(line[0]))
    {
        std::cout << "(then" << std::endl;
        processLine(lines, stringMap, exitStack, lineNumber, true); // process the same line, strip the then label
    }
    else if (line[0] == "ENDTHEN" || line[0] == "ENDELSE")
    {
        std::cout << ")" << std::endl;
    }
    else if (isElseLabel(line[0]))
    {
        std::cout << "(else" << std::endl;
        processLine(lines, stringMap, exitStack, lineNumber, true); // process the same line, strip the else label
        
    }
    else if (isEndIfLabel(line[0]))
    {
        std::cout << ")" << std::endl;
    }
    else if (line[0] == "CALL" && line[1] == "PRINT" && line[2][0] != '$')
    { // normal logging - i.e. integer or variable
        std::string src = line[2];
        printOperand(src);
        std::cout << "call $log" << std::endl;
    }
    else if (line[0] == "CALL" && line[1] == "READ")
    {
        std::cout << "call $read" << std::endl;
        std::cout << "local.set $" << line[2] << std::endl;
    }
    else if (line[0] == "CALL" && line[1] == "PRINT" && line[2][0] == '$')
    {
        std::string src = line[2];
        unsigned long start_offset = stringMap[src][0];
        unsigned long length = stringMap[src][1];
        std::cout << "i32.const " << start_offset << std::endl;
        std::cout << "i32.const " << length << std::endl;
        std::cout << "call $logString" << std::endl;
    }
    else if (line[0] == "LOOP")
    {
        std::string bodyLabel = line[1];
        std::string exitLabel = line[2];

        std::cout << "(block $" << exitLabel << std::endl;
        std::cout << "(loop $" << bodyLabel << std::endl;

        exitStack.push_back(exitLabel);

        return processLine(lines, stringMap, exitStack, lineNumber + 1, true); // process the next line, strip the body label
        ////NOTE: we are returning in this case - since we move to the NEXT line
    }
    else if (line[0] == "EXIT")
    {
        
        std::string exitLabel = exitStack.back();
        // exitStack.pop_back();

        std::cout << "br $" << exitLabel << std::endl;
    }
    else if (isLoopExitLabel(line[0]))
    {
        std::string exitLabel = line[0];
        std::string loopBodyLabel = "body" + exitLabel.substr(4, exitLabel.size() - 1 - 4); // e.g. exit1 -> body1

        std::cout << "br $" << loopBodyLabel << "\n)\n)" << std::endl;
    }

    else
    {
        for (auto token : line)
        {
            std::cout << token << " ";
        }
        std::cout << std::endl;
    }

    return lineNumber + 1;
}