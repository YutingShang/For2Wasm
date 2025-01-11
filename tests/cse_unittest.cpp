#include "unittest_utils.h"

using namespace antlr4;

//// Test suites in this file:
// CSEOptimisationTest - tests related to common subexpression elimination
// CSE_DCEOptimisationTest - tests related to common subexpression elimination and dead code elimination

//////////////////////// Helper function declarations ////////////////////////

void run_CSE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_CSE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);
void run_CSE_DCE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_CSE_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//////////////////////// Tests ///////////////////////////////////////////////
TEST(CSEOptimisationTest, Common1ProgramCSEIR)
{
    run_CSE_IRStringTest("common1.f90", "common1_CSE_IR.txt");
}

TEST(CSEOptimisationTest, Common1ProgramCSEWASM)
{
    run_CSE_WASMTest("common1.f90", "common1_CSE.wat");
}

TEST(CSEOptimisationTest, Common2ProgramCSEIR)
{
    run_CSE_IRStringTest("common2.f90", "common2_CSE_IR.txt");
}

TEST(CSEOptimisationTest, Common2ProgramCSEWASM)
{
    run_CSE_WASMTest("common2.f90", "common2_CSE.wat");
}

TEST(CSEOptimisationTest, Common3ProgramCSEIR)
{
    run_CSE_IRStringTest("common3.f90", "common3_CSE_IR.txt");
}

TEST(CSEOptimisationTest, Common3ProgramCSEWASM)
{
    run_CSE_WASMTest("common3.f90", "common3_CSE.wat");
}

//cse_dce tests

TEST(CSE_DCEOptimisationTest, Common1ProgramCSE_DCEIR)
{
    run_CSE_DCE_IRStringTest("common1.f90", "common1_CSE_IR.txt");       //dce should not affect common1 program
}

TEST(CSE_DCEOptimisationTest, Common1ProgramCSE_DCEWASM)
{
    run_CSE_DCE_WASMTest("common1.f90", "common1_CSE.wat");       //dce should not affect common1 program
}

TEST(CSE_DCEOptimisationTest, Common2ProgramCSE_DCEIR)
{
    run_CSE_DCE_IRStringTest("common2.f90", "common2_CSE_DCE_IR.txt");
}

TEST(CSE_DCEOptimisationTest, Common2ProgramCSE_DCEWASM)
{
    run_CSE_DCE_WASMTest("common2.f90", "common2_CSE_DCE.wat");
}

TEST(CSE_DCEOptimisationTest, Common3ProgramCSE_DCEIR)
{
    run_CSE_DCE_IRStringTest("common3.f90", "common3_CSE_IR.txt");       //dce should not affect common3 program
}

TEST(CSE_DCEOptimisationTest, Common3ProgramCSE_DCEWASM)
{
    run_CSE_DCE_WASMTest("common3.f90", "common3_CSE.wat");       //dce should not affect common3 program
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

void run_CSE_DCE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {CSE, DCE});
}

void run_CSE_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {CSE, DCE});
}


