#pragma once
#include "antlr4-runtime.h"
#include "Fortran90ParserBaseVisitor.h"

class Fortran90ParserCustomVisitor : public Fortran90ParserBaseVisitor {
public:
    virtual std::any visitChildren(antlr4::tree::ParseTree *node) override;

    // virtual std::any visitProgram(Fortran90Parser::ProgramContext *ctx) override;
};
