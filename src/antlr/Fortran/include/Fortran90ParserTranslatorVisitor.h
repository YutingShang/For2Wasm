#pragma once
#include "antlr4-runtime.h"
#include "Fortran90ParserBaseVisitor.h"
#include <algorithm>  // for std::transform
#include <cctype>     // for std::tolower

class Fortran90ParserTranslatorVisitor : public Fortran90ParserBaseVisitor
{
public:
    // constructor
    Fortran90ParserTranslatorVisitor(Fortran90Parser &parser) : parser(parser) {};

    virtual std::any visitChildren(antlr4::tree::ParseTree *node) override;

    virtual std::any visitTerminal(antlr4::tree::TerminalNode *node) override;

    virtual std::any visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx) override;

    virtual std::any visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx) override;

    virtual std::any visitAddOperand(Fortran90Parser::AddOperandContext *ctx) override;

    virtual std::any visitPrimary(Fortran90Parser::PrimaryContext *ctx) override;

    virtual std::any visitPrintStmt(Fortran90Parser::PrintStmtContext *ctx) override;

    virtual std::any visitOutputItemList1(Fortran90Parser::OutputItemList1Context *ctx) override;

    virtual std::any visitReadStmt(Fortran90Parser::ReadStmtContext *ctx) override;

    virtual std::any visitCommaInputItemList(Fortran90Parser::CommaInputItemListContext *ctx) override;

    virtual std::any visitInputItemList(Fortran90Parser::InputItemListContext *ctx) override;

    virtual std::any visitTypeDeclarationStmt(Fortran90Parser::TypeDeclarationStmtContext *ctx) override;

    virtual std::any visitEntityDeclList(Fortran90Parser::EntityDeclListContext *ctx) override;

    virtual std::any visitEquivOperand(Fortran90Parser::EquivOperandContext *ctx) override;

    virtual std::any visitOrOperand(Fortran90Parser::OrOperandContext *ctx) override;

    virtual std::any visitAndOperand(Fortran90Parser::AndOperandContext *ctx) override;

    virtual std::any visitLevel4Expr(Fortran90Parser::Level4ExprContext *ctx) override;

    virtual std::any visitIfConstruct(Fortran90Parser::IfConstructContext *ctx) override;

    /// wrapper functions - QUESTION: private or public?
    virtual void visitIfThenStmtWithLabel(Fortran90Parser::IfThenStmtContext *ctx, const std::string &condLabel, const std::string &thenLabel);

    virtual std::any visitIfThenStmt(Fortran90Parser::IfThenStmtContext *ctx) override;

    // virtual std::any visitConditionalBody(Fortran90Parser::ConditionalBodyContext *ctx) override;

    virtual std::any visitElseIfConstruct(Fortran90Parser::ElseIfConstructContext *ctx) override;

    virtual std::any visitElseIfStmt(Fortran90Parser::ElseIfStmtContext *ctx) override;

    virtual std::any visitElseConstruct(Fortran90Parser::ElseConstructContext *ctx) override;

    virtual std::any visitElseStmt(Fortran90Parser::ElseStmtContext *ctx) override;

    virtual std::any visitEndIfStmt(Fortran90Parser::EndIfStmtContext *ctx) override;

    

private:
    // the parser to access the rule names
    Fortran90Parser &parser;

    int tempVariableCount = 0;            // just to keep track of temporary variables used for intermediate results - not mapped to any user defined variables
    std::string getNewTempVariableName(); // fresh temp variable name

    //<index> <string_to_print>
    // store strings to be printed as '$str1', '$str2', etc:
    // will strip the quotes of the "strings_to_print"
    std::unordered_map<std::string, std::string> stringIndexMap;
    std::string getStringIndex(std::string str);
    std::string getItemToPrint(std::string outputItem);       // check if the outputItem is a string or a variable/number etc.
    std::string getRelationalOperator(std::string operation); // get the relational operator, convert from <, > to LT, GT etc.
    int conditionCount = 0;      //for naming the block labels
    int thenCount = 0;
    int elseCount = 0;

    


    
};