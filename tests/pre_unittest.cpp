#include "unittest_utils.h"

//// Test suites in this file:
// PREOptimisationTest - tests related to partial redundancy elimination, including further iterations of PRE + CP, and const + DCE

//////////////////////// Helper function declarations ////////////////////////

void run_PRE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);
void run_iterPRE_CP_WASMTest(std::string inputFileName, std::string expectedOutputFileName);
void run_iterPRE_CP_const_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//////////////////////// Tests ///////////////////////////////////////////////
TEST(PREOptimisationTest, PartialRedundant1ProgramPREWASM)
{
    run_PRE_WASMTest("partialredundant1.f90", "partialredundant1_PRE.wat");
}

TEST(PREOptimisationTest, PartialRedundant1ProgramIterPRE_CP_const_DCEWASM)
{
    run_iterPRE_CP_const_DCE_WASMTest("partialredundant1.f90", "partialredundant1_iterPRE-CP_const_DCE.wat");
}

TEST(PREOptimisationTest, PartialRedundant2ProgramIterPRE_CPWASM)
{
    run_iterPRE_CP_WASMTest("partialredundant2.f90", "partialredundant2_iterPRE-CP.wat");
}

TEST(PREOptimisationTest, PartialRedundant2ProgramIterPRE_CP_const_DCEWASM)
{
    run_iterPRE_CP_const_DCE_WASMTest("partialredundant2.f90", "partialredundant2_iterPRE-CP_const_DCE.wat");
}

TEST(PREOptimisationTest, PartialRedundant3ProgramIterPRE_CP_const_DCEWASM)
{
    run_iterPRE_CP_const_DCE_WASMTest("partialredundant3.f90", "partialredundant3_iterPRE-CP_const_DCE.wat");
}

TEST(PREOptimisationTest, PartialRedundant4ProgramIterPRE_CPWASM)
{
    run_iterPRE_CP_WASMTest("partialredundant4.f90", "partialredundant4_iterPRE-CP.wat");
}

TEST(PREOptimisationTest, Common4ProgramIterPRE_CPWASM)
{
    run_iterPRE_CP_WASMTest("common4.f90", "common4_iterPRE-CP.wat");
}

TEST(PREOptimisationTest, GameOfLifeProgramPREWASM)
{
    run_PRE_WASMTest("gameOfLife.f90", "gameOfLife_PRE.wat");
}


//////////////////////// Helper function definitions ////////////////////////

void run_PRE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {PRE});
}

void run_iterPRE_CP_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP});
}

void run_iterPRE_CP_const_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP, Const, DCE});
}









