#pragma once

#include <string>
#include <gtest/gtest.h>
#include <fstream>
#include <antlr4-runtime.h>
#include "Fortran90Lexer.h"
#include "Fortran90Parser.h"
#include "Fortran90ParserASTVisitor.h"
#include "Fortran90ParserIRTreeVisitor.h"
#include "IrFlowgraphVisitor.h"
#include "DeadCodeElimination.h"
#include "SimplificationOptimisations.h"
#include "CSEOptimizer.h"
#include "IrWasmVisitor.h"
#include "EntryNode.h"

//global variable - extern definition to avoid duplication
extern std::string relativeProjectRootPath;      //from where the makefile is run

enum OptimisationFlag {
    DCE,
    Simplify,
    CSE
};

enum OutputFlag {
    IR,
    WASM,
    AST
};

//custom pipeline test, used to construct the other helper functions
void run_custom_pipeline_test(std::string inputFileName, std::string expectedOutputFileName, OutputFlag outputFlag, std::vector<OptimisationFlag> optimisationFlags);
