#include "unittest_utils.h"

//// Test suites in this file:
// CSEOptimisationTest - tests related to common subexpression elimination
// CSE_DCEOptimisationTest - tests related to common subexpression elimination and dead code elimination
// CSE_IterCSE_CPOptimisationTest - tests related to common subexpression elimination and copy propagation
// CSE_ConstOptimisationTest - tests related to common subexpression elimination and constant propagation

//////////////////////// Helper function declarations ////////////////////////

void run_CSE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_CSE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//dce alone should not affect these programs
void run_CSE_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//combine iterCSE-CP / iterCSE-CP -const / iterCSE-CP -const -DCE
void run_iterCSE_CP_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_iterCSE_CP_const_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_iterCSE_CP_const_DCE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);

//running const before or after iterCSE_CP should not affect the output
void run_const_iterCSE_CP_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);
void run_iterCSE_CP_const_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//CSE then CP once
void run_CSE_CP_WASMTest(std::string inputFileName, std::string expectedOutputFileName);


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

//cse_dce tests - dce alone should not affect these programs

TEST(CSE_DCEOptimisationTest, Common1ProgramCSE_DCEWASM)
{
    run_CSE_DCE_WASMTest("common1.f90", "common1_CSE.wat");       
}

TEST(CSE_DCEOptimisationTest, Common2ProgramCSE_DCEWASM)
{
    run_CSE_DCE_WASMTest("common2.f90", "common2_CSE.wat");     
}

TEST(CSE_DCEOptimisationTest, Common3ProgramCSE_DCEWASM)
{
    run_CSE_DCE_WASMTest("common3.f90", "common3_CSE.wat");      
}

TEST(CSEOptimisationTest, Common4ProgramCSEWASM)
{
    run_CSE_WASMTest("common4.f90", "common4_CSE.wat");
}

//iterCSE_CP tests

//common 1, with iterCSE-CP and DCE
TEST(CSE_IterCSE_CPOptimisationTest, Common1ProgramIterCSE_CP_DCEWASM)
{
    run_iterCSE_CP_const_DCE_WASMTest("common1.f90", "common1_CSE_CP_const_DCE.wat");    //just run CSE and CP once here in the iterCSE-CP
}

//common 2 with various combinations

TEST(CSE_IterCSE_CPOptimisationTest, Common2ProgramIterCSE_CPIR)
{
    run_iterCSE_CP_IRStringTest("common2.f90", "common2_iterCSE-CP_IR.txt");
}

TEST(CSE_IterCSE_CPOptimisationTest, Common2ProgramIterCSE_CP_constIR)
{
    run_iterCSE_CP_const_IRStringTest("common2.f90", "common2_iterCSE-CP_const_IR.txt");
}

TEST(CSE_IterCSE_CPOptimisationTest, Common2ProgramIterCSE_CP_const_DCEIR)
{
    run_iterCSE_CP_const_DCE_IRStringTest("common2.f90", "common2_iterCSE-CP_const_DCE_IR.txt");
}

TEST(CSE_IterCSE_CPOptimisationTest, Common2ProgramIterCSE_CP_const_DCEWASM)
{
    run_iterCSE_CP_const_DCE_WASMTest("common2.f90", "common2_iterCSE-CP_const_DCE.wat");
}

TEST(CSE_IterCSE_CPOptimisationTest, Common2ProgramIterCSE_CP_DCEWASM)
{
    run_const_iterCSE_CP_DCE_WASMTest("common2.f90", "common2_iterCSE-CP_const_DCE.wat");    //const before iterCSE_CP should not affect the output
}

// a regression test for fixed error - LoadElt and StoreElt did not have replaceReferencedVariable implemented
TEST(CSE_IterCSE_CPOptimisationTest, GameOfLifeProgramCSE_CP_WASM)
{
    run_CSE_CP_WASMTest("gameOfLife.f90", "gameOfLife_CSE_CP.wat");
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

void run_CSE_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {CSE, DCE});
}


void run_iterCSE_CP_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {IterCSE_CP});
}

void run_iterCSE_CP_const_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {IterCSE_CP, Const});
}

void run_iterCSE_CP_const_DCE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {IterCSE_CP, Const, DCE});
}

void run_const_iterCSE_CP_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {Const, IterCSE_CP, DCE});
}

void run_iterCSE_CP_const_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {IterCSE_CP, Const, DCE});
}

void run_CSE_CP_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {CSE, CP});
}


