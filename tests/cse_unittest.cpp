#include "unittest_utils.h"

using namespace antlr4;

//// Test suites in this file:
// CSEOptimisationTest - tests related to common subexpression elimination

//////////////////////// Helper function declarations ////////////////////////

void run_CSE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_CSE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//////////////////////// Tests ///////////////////////////////////////////////
TEST(CSEOptimisationTest, Common1ProgramCSEIR)
{
    run_CSE_IRStringTest("common1.f90", "common1_CSE_IR.txt");
}

TEST(CSEOptimisationTest, Common1ProgramCSEWASM)
{
    run_CSE_WASMTest("common1.f90", "common1_CSE.wat");
}

//////////////////////// Helper function definitions ////////////////////////

void run_CSE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {CSE});
}

void run_CSE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {CSE});
}




