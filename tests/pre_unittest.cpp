#include "unittest_utils.h"

//// Test suites in this file:
// PREOptimisationTest - tests related to partial redundancy elimination, including further iterations of PRE + CP, and const + DCE

//////////////////////// Helper function declarations ////////////////////////

void run_PRE_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version=1);
void run_iterPRE_CP_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version=1);
void run_iterPRE_CP_const_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version=1);
void run_iterPRE_CP_DCE_simplify_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version=1);

//////////////////////// Tests  VERSION 1///////////////////////////////////////////////
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

TEST(PREOptimisationTest, ImageFilterProgramIterPRE_CP_DCE_simplifyWASM)
{
    run_iterPRE_CP_DCE_simplify_WASMTest("imageFilter.f90", "imageFilter_iterPRE_CP_DCE_simplify.wat");
}

//////////////////////// Tests  VERSION 2///////////////////////////////////////////////


TEST(PREOptimisationTest, PartialRedundant1ProgramPREWASM_2)
{
    run_PRE_WASMTest("partialredundant1.f90", "partialredundant1_PRE.wat", 2);
}

TEST(PREOptimisationTest, PartialRedundant1ProgramIterPRE_CP_const_DCEWASM_2)
{
    run_iterPRE_CP_const_DCE_WASMTest("partialredundant1.f90", "partialredundant1_iterPRE-CP_const_DCE.wat", 2);
}

TEST(PREOptimisationTest, PartialRedundant2ProgramIterPRE_CPWASM_2)
{
    run_iterPRE_CP_WASMTest("partialredundant2.f90", "partialredundant2_iterPRE-CP.wat", 2);
}

TEST(PREOptimisationTest, PartialRedundant2ProgramIterPRE_CP_const_DCEWASM_2)
{
    run_iterPRE_CP_const_DCE_WASMTest("partialredundant2.f90", "partialredundant2_iterPRE-CP_const_DCE.wat", 2);
}

TEST(PREOptimisationTest, PartialRedundant3ProgramIterPRE_CP_const_DCEWASM_2)
{
    run_iterPRE_CP_const_DCE_WASMTest("partialredundant3.f90", "partialredundant3_iterPRE-CP_const_DCE.wat", 2);
}

TEST(PREOptimisationTest, PartialRedundant4ProgramIterPRE_CPWASM_2)
{
    run_iterPRE_CP_WASMTest("partialredundant4.f90", "partialredundant4_iterPRE-CP.wat", 2);
}

TEST(PREOptimisationTest, Common4ProgramIterPRE_CPWASM_2)
{
    run_iterPRE_CP_WASMTest("common4.f90", "common4_iterPRE-CP.wat", 2);
}

TEST(PREOptimisationTest, GameOfLifeProgramPREWASM_2)
{
    run_PRE_WASMTest("gameOfLife.f90", "gameOfLife_PRE.wat", 2);
}

TEST(PREOptimisationTest, ImageFilterProgramIterPRE_CP_DCE_simplifyWASM_2)
{
    run_iterPRE_CP_DCE_simplify_WASMTest("imageFilter.f90", "imageFilter_iterPRE_CP_DCE_simplify.wat", 2);
}


//////////////////////// Helper function definitions ////////////////////////

void run_PRE_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version)
{
    if (version == 1)
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {PRE});
    else
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {PRE_2});
}

void run_iterPRE_CP_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version)
{
    if (version == 1)
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP});
    else
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP_2});
}

void run_iterPRE_CP_const_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version)
{
    if (version == 1)
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP, Const, DCE});
    else
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP_2, Const, DCE});
}

void run_iterPRE_CP_DCE_simplify_WASMTest(std::string inputFileName, std::string expectedOutputFileName, int version)
{
    if (version == 1)
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP, DCE, Simplify});
    else
        run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterPRE_CP_2, DCE, Simplify});
}









