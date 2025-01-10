#include "unittest_utils.h"


using namespace antlr4;

//// Test suites in this file:
// DCEOptimisationTest - tests related to dead code elimination
// DCESimplificationOptimisationTest - tests related to dead code elimination with simplification

//////////////////////// Helper function declarations ////////////////////////

void run_DCE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName);
void run_DCE_simp_IRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void run_DCE_simp_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//////////////////////// Tests ///////////////////////////////////////////////
TEST(DCEOptimisationTest, Dead1ProgramDCEIR)
{
    run_DCE_IRStringTest("dead1.f90", "dead1_DCE_IR.txt");
}

TEST(DCEOptimisationTest, Dead2ProgramDCEIR)
{
    run_DCE_IRStringTest("dead2.f90", "dead2_DCE_IR.txt");
}

TEST(DCEOptimisationTest, Dead3ProgramDCEIR)
{
    run_DCE_IRStringTest("dead3.f90", "dead3_DCE_IR.txt");
}

TEST(DCEOptimisationTest, Dead1ProgramDCEWASM)
{
    run_DCE_WASMTest("dead1.f90", "dead1_DCE.wat");
}

TEST(DCEOptimisationTest, Dead2ProgramDCEWASM)
{
    run_DCE_WASMTest("dead2.f90", "dead2_DCE.wat");
}

TEST(DCEOptimisationTest, Dead3ProgramDCEWASM)
{
    run_DCE_WASMTest("dead3.f90", "dead3_DCE.wat");
}

//with simplification

TEST(DCESimplificationOptimisationTest, Dead1ProgramSimplificationIR)
{
    run_DCE_simp_IRStringTest("dead1.f90", "dead1_DCE_IR.txt");      //simplification does not affect dead1 program
}

TEST(DCESimplificationOptimisationTest, Dead2ProgramSimplificationIR)
{
    run_DCE_simp_IRStringTest("dead2.f90", "dead2_DCE_simp_IR.txt");      
}

TEST(DCESimplificationOptimisationTest, Dead3ProgramSimplificationIR)
{
    run_DCE_simp_IRStringTest("dead3.f90", "dead3_DCE_simp_IR.txt");      
}

TEST(DCESimplificationOptimisationTest, Dead1ProgramSimplificationWASM)
{
    run_DCE_simp_WASMTest("dead1.f90", "dead1_DCE.wat");      //simplification does not affect dead1 program
}

TEST(DCESimplificationOptimisationTest, Dead2ProgramSimplificationWASM)
{
    run_DCE_simp_WASMTest("dead2.f90", "dead2_DCE_simp.wat");      
}

TEST(DCESimplificationOptimisationTest, Dead3ProgramSimplificationWASM)
{
    run_DCE_simp_WASMTest("dead3.f90", "dead3_DCE_simp.wat");     
}


//////////////////////// Helper function definitions ////////////////////////

void run_DCE_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {DCE});
}

void run_DCE_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {DCE});
}

void run_DCE_simp_IRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {DCE, Simplify});
}

void run_DCE_simp_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {DCE, Simplify});
}




