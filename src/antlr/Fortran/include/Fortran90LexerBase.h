#pragma once

#include "antlr4-runtime.h"

class Fortran90LexerBase : public antlr4::Lexer
{
public:
    Fortran90LexerBase(antlr4::CharStream *input) : Lexer(input), self(*this) { }

public:
    Fortran90LexerBase & self;

public:
    bool IsColumnZero();
    bool VerifyNotOperator();
};

