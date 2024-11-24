#pragma once

#include "antlr4-runtime.h"
#include "Fortran90Parser.h"

class Fortran90ContextCopy
{
public:
    static antlr4::ParserRuleContext *copyNode(antlr4::ParserRuleContext *nodeToCopy);
};

