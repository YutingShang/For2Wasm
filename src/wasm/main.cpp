#include <iostream>
#include <fstream>
#include <sstream>

void printOperand(const std::string &operand);

///NOTE: this will return the next line number to process
int processLine(const std::vector<std::vector<std::string>> &lines, const int lineNumber, bool stripLabel); 

int main(int, const char **)
{
    std::ifstream inFile("../antlr/Fortran/output.ir");

    if (!inFile.is_open())
    {
        std::cerr << "Failed to open input file" << std::endl;
        return 1;
    }

    // read the entire file into a string
    // std::ostringstream buffer;
    // buffer << inFile.rdbuf();     //read file into memory buffer
    // std::string content(buffer.str());  //convert buffer into string

    std::vector<std::vector<std::string>> lines;

    // process each line of the file into 2D array of tokens
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

    // print the lines
    // for (auto line: lines){
    //     for (auto token: line){
    //         std::cout << token << " ";
    //     }
    //     std::cout << std::endl;
    // }

    int lineNumber = 0;
    while (lineNumber < lines.size())
    {
        lineNumber = processLine(lines, lineNumber, false);    //will get the next line number to process
    }

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
    return !s.empty() && s.back() == ':' && s.substr(0,4) == "cond";
}

bool isThenLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0,4) == "then";
}

bool isElseLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0,4) == "else";
}

bool isEndIfLabel(const std::string &s)
{
    return !s.empty() && s.back() == ':' && s.substr(0,5) == "endif";
}

bool isLabel(const std::string &s)
{
    return isCondLabel(s) || isThenLabel(s) || isElseLabel(s);   //add other labels too
}





int processLine(const std::vector<std::vector<std::string>> &lines, const int lineNumber, bool stripLabel)
{
    std::vector<std::string> line = lines[lineNumber];
    if (stripLabel && isLabel(line[0])){
        line.erase (line.begin());      //remove the label
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
    else if (line[0] == "ADD")
    {
        // in my AST, ADD will always be ADD <tmp_dest> <src1> <src2>
        // so in wasm, the temp_dest will just be implicit on the stack

        // need to check operand is integer or string var
        /// TODO: parse the labels of the IR - check if it ends with :
        // maybe put into a dictionary map?? or not

        std::string src1 = line[2];
        std::string src2 = line[3];

        // if (isPosInteger(src1)){
        //     std::cout << "(i32.const " << src1 << ") ";
        // }else if (isVariable(src1)){
        //     std::cout << "(local.get $"<<src1<<") ";
        // }//otherwise its a temporary, don't print it out

        printOperand(src1);
        printOperand(src2);
        std::cout << "i32.add" << std::endl;
    }
    else if (line[0]=="IF"){
        // process condition - go until "TEST <var>"

        // process the then block, go until "ENDTHEN"

        // process the else block if it exists, go until "ENDELSE"

        // print nothing when we actually see this "IF"

    }

    else if (isCondLabel(line[0])){
        processLine(lines, lineNumber, true);    //process the same line, strip the cond label
    }
    else if (line[0]=="TEST"){

        ///NOTE: this is assuming cond labels are only used for 'IF' statements
        std::cout << "(if" << std::endl;


    }else if (line[0]=="EQ"){     
        //result will be saved to a temp var - implicit on the stack
        std::string src1 = line[2];
        std::string src2 = line[3];

        printOperand(src1);
        printOperand(src2);
        std::cout << "i32.eq" << std::endl;

    }else if (isThenLabel(line[0])){
        std::cout <<"(then"<<std::endl;
        processLine(lines, lineNumber, true);    //process the same line, strip the then label
    }else if (line[0]=="ENDTHEN" || line[0]=="ENDELSE"){ 
        std::cout <<")"<<std::endl;
    }else if (isElseLabel(line[0])){
        std::cout <<"(else"<<std::endl;
        processLine(lines, lineNumber, true);    //process the same line, strip the else label
    }else if (isEndIfLabel(line[0])){
        std::cout <<")"<<std::endl;
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