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

    virtual std::any visitAddOperand(Fortran90Parser::AddOperandContext *ctx) override;

    // virtual std::any visitMultOperand(Fortran90Parser::MultOperandContext *ctx) override;

    // virtual std::any visitLevel1Expr(Fortran90Parser::Level1ExprContext *ctx) override;

    // virtual std::any visitDefinedUnaryOp(Fortran90Parser::DefinedUnaryOpContext *ctx) override;

    virtual std::any visitPrimary(Fortran90Parser::PrimaryContext *ctx) override;


private:
    //the parser to access the rule names
    Fortran90Parser &parser;

    int tempVariableCount=0; 

    // std::unordered_map<std::string, std::string> tempVariableMap;      //actually name might be misleading, more like a VARIRABLE_MAP/context, not for all temp variables, but for all user defined variables

    // std::string getTempVariableName(std::string variableName);   // get temp variable name for a user defined variable, or create a new one if it doesn't exist
    std::string getNewTempVariableName();      // fresh temp variable name
};
