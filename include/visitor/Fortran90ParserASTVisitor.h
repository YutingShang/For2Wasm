#pragma once
#include "antlr4-runtime.h"
#include "Fortran90ParserBaseVisitor.h"

class Fortran90ParserASTVisitor : public Fortran90ParserBaseVisitor {
public:
    //constructor
    Fortran90ParserASTVisitor(Fortran90Parser &parser){}; //: parser(parser){};

    virtual std::any visitChildren(antlr4::tree::ParseTree *node) override;

    virtual std::any visitTerminal(antlr4::tree::TerminalNode *node) override;


private:
    //the parser
    // Fortran90Parser &parser;

};
