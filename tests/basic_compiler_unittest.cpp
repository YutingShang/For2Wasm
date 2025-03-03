#include "unittest_utils.h"


//// Test suites in this file:
// IRTranslationTest - tests related to translating Fortran to AST and IR
// WASMTranslationTest - tests related to translating Fortran to WASM

//////////////////////// Helper function declarations ////////////////////////

void runASTStringTest(std::string inputFileName, std::string expectedOutputFileName);
void runIRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void runWASMTest(std::string inputFileName, std::string expectedOutputFileName);


//////////////////////// Tests ///////////////////////////////////////////////
TEST(IRTranslationTest, SummationProgramAST)
{
    runASTStringTest("summation.f90", "summation_AST.txt");
}

TEST(IRTranslationTest, FactorialProgramAST)
{
    runASTStringTest("factorial.f90", "factorial_AST.txt");
}

TEST(IRTranslationTest, TriangularProgramAST)
{
    runASTStringTest("triangular.f90", "triangular_AST.txt");
}

TEST(IRTranslationTest, SummationProgramIR)
{
    runIRStringTest("summation.f90", "summation_IR.txt");
}

TEST(IRTranslationTest, FactorialProgramIR)
{
    runIRStringTest("factorial.f90", "factorial_IR.txt");
}

TEST(IRTranslationTest, TriangularProgramIR)
{
    runIRStringTest("triangular.f90", "triangular_IR.txt");
}

TEST(IRTranslationTest, Dead1ProgramIR)
{
    runIRStringTest("dead1.f90", "dead1_IR.txt");
}

TEST(IRTranslationTest, Dead2ProgramIR)
{
    runIRStringTest("dead2.f90", "dead2_IR.txt");
}

TEST(IRTranslationTest, Dead3ProgramIR)
{
    runIRStringTest("dead3.f90", "dead3_IR.txt");
}

TEST(IRTranslationTest, Common1ProgramIR)
{
    runIRStringTest("common1.f90", "common1_IR.txt");
}

TEST(IRTranslationTest, ControlFlowProgramIR)
{
    runIRStringTest("controlFlow.f90", "controlFlow_IR.txt");
}


TEST(WASMTranslationTest, SummationProgramWASM)
{
    runWASMTest("summation.f90", "summation.wat");
}

TEST(WASMTranslationTest, FactorialProgramWASM)
{
    runWASMTest("factorial.f90", "factorial.wat");
}

TEST(WASMTranslationTest, TriangularProgramWASM)
{
    runWASMTest("triangular.f90", "triangular.wat");
}

TEST(WASMTranslationTest, ArithmeticProgramWASM)
{
    runWASMTest("arithmetic.f90", "arithmetic.wat");
}

TEST(WASMTranslationTest, Arithmetic2ProgramWASM)
{
    runWASMTest("arithmetic2.f90", "arithmetic2.wat");
}

TEST(WASMTranslationTest, Array1ProgramWASM)
{
    runWASMTest("array1.f90", "array1.wat");
}

TEST(WASMTranslationTest, Array2ProgramWASM)
{
    runWASMTest("array2.f90", "array2.wat");
}

TEST(WASMTranslationTest, ControlFlowProgramWASM)
{
    runWASMTest("controlFlow.f90", "controlFlow.wat");
}

TEST(WASMTranslationTest, BinomialCoeffProgramWASM)
{
    runWASMTest("binomialCoeff.f90", "binomialCoeff.wat");
}

//////////////////////// Helper function definitions ////////////////////////

void runASTStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, AST, {});
}

void runIRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, IR, {});
}

void runWASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {});
}