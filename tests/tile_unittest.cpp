#include "unittest_utils.h"

//// Test suites in this file:
// LoopTilingTest - tests related to loop tiling

//////////////////////// Helper function declarations ////////////////////////

void run_tiled_WASMTest(std::string inputFileName, std::string expectedOutputFileName);

//////////////////////// Tests ///////////////////////////////////////////////
TEST(LoopTilingTest, Matrix2000ProgramTiledWASM)
{
    run_tiled_WASMTest("matrix2000.f90", "matrix2000_tiled.wat");
}

TEST(LoopTilingTest, Matrix1ProgramTiledWASM)
{
    run_tiled_WASMTest("matrix1.f90", "matrix1_tiled.wat");
}

TEST(LoopTilingTest, Matrix2ProgramTiledWASM)
{
    run_tiled_WASMTest("matrix2.f90", "matrix2_tiled.wat");
}

TEST(LoopTilingTest, Matrix3ProgramTiledWASM)
{
    run_tiled_WASMTest("matrix3.f90", "matrix3_tiled.wat");
}


//////////////////////// Helper function definitions ////////////////////////

void run_tiled_WASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
    run_custom_pipeline_test(inputFileName, expectedOutputFileName, WASM, {Tile});
}








