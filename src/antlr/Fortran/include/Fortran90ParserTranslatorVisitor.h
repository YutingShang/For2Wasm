#pragma once
#include "antlr4-runtime.h"
#include "Fortran90ParserBaseVisitor.h"

class Fortran90ParserTranslatorVisitor : public Fortran90ParserBaseVisitor {
public:
    //constructor
    Fortran90ParserTranslatorVisitor(Fortran90Parser &parser): parser(parser){};

    virtual std::any visitChildren(antlr4::tree::ParseTree *node) override;

    virtual std::any visitTerminal(antlr4::tree::TerminalNode *node) override;

     virtual std::any visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx) override;

     virtual std::any visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx) override;
    

private:
    //the parser to access the rule names
    Fortran90Parser &parser;

    int tempVariableCount=0; 

    std::unordered_map<std::string, std::string> tempVariableMap;      //actually name might be misleading, more like a VARIRABLE_MAP/context, not for all temp variables, but for all user defined variables

    std::string getTempVariableName(std::string variableName);   // get temp variable name for a user defined variable, or create a new one if it doesn't exist
    std::string getNewTempVariableName();      // fresh temp variable name
};
