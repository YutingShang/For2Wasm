#pragma once

#include <string>
#include <gtest/gtest.h>


//global variable - extern definition to avoid duplication
extern std::string relativeProjectRootPath;      //from where the makefile is run

enum OptimisationFlag {
    DCE,
    Simplify,
    CSE,
    CP,
    Const,
    IterCSE_CP,
};

enum OutputFlag {
    IR,
    WASM,
    AST
};

//custom pipeline test, used to construct the other helper functions
void run_custom_pipeline_test(std::string inputFileName, std::string expectedOutputFileName, OutputFlag outputFlag, std::vector<OptimisationFlag> optimisationFlags);
