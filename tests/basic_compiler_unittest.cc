#include <gtest/gtest.h>
#include <fstream>
#include <antlr4-runtime.h>
#include "Fortran90Lexer.h"
#include "Fortran90Parser.h"
#include "Fortran90ParserASTVisitor.h"
#include "Fortran90ParserIRTreeVisitor.h"
#include "IrWasmVisitor.h"
#include "EntryNode.h"


// using namespace antlrcpp;
using namespace antlr4;

//// Test suites in this file:
// IRTranslationTest - tests related to translating Fortran to AST and IR
// WASMTranslationTest - tests related to translating Fortran to WASM

std::string relativeProjectRootPath = "./";      //from where the makefile is run


//////////////////////// Helper function declarations ////////////////////////
void runASTStringTest(std::string inputFileName, std::string expectedOutputFileName);
void runIRStringTest(std::string inputFileName, std::string expectedOutputFileName);
void runWASMTest(std::string inputFileName, std::string expectedOutputFileName);


//////////////////////// Tests ///////////////////////////////////////////////
TEST(IRTranslationTest, SummationProgramAST)
{
    runASTStringTest("summation.f90", "summationAST.txt");
}

TEST(IRTranslationTest, FactorialProgramAST)
{
    runASTStringTest("factorial.f90", "factorialAST.txt");
}

TEST(IRTranslationTest, TriangularProgramAST)
{
    runASTStringTest("triangular.f90", "triangularAST.txt");
}

TEST(IRTranslationTest, SummationProgramIR)
{
    runIRStringTest("summation.f90", "summationIR.txt");
}

TEST(IRTranslationTest, FactorialProgramIR)
{
    runIRStringTest("factorial.f90", "factorialIR.txt");
}

TEST(IRTranslationTest, TriangularProgramIR)
{
    runIRStringTest("triangular.f90", "triangularIR.txt");
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

//////////////////////// Helper function definitions ////////////////////////
void runASTStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    //reading the input file
    std::ifstream inputFile;
    inputFile.open(relativeProjectRootPath + "examples/" + inputFileName); // open the Fortran program to translate to IR, e.g: ./main examples/summation.f90
    if (!inputFile.is_open())
    {
        FAIL() << "Failed to open input file"; // custom failure message
    }
    ANTLRInputStream input(inputFile);
    inputFile.close();

    //lexing and parsing
    Fortran90Lexer lexer(&input);
    CommonTokenStream tokens(&lexer); // create stream of tokens with lexer
    tokens.fill();                    // process all tokens before printing
    Fortran90Parser parser(&tokens);
    tree::ParseTree *parseTree = parser.program();
   
    //AST visitor
    Fortran90ParserASTVisitor astVisitor;
    antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree *>(parseTree->accept(&astVisitor)); // dispatches call to appropriate visit method in visitor

    //saving the AST string
    std::string output = astTree->toStringTree(&parser);

    //comparing the output with the expected output
    std::ifstream expectedOutputFile(relativeProjectRootPath + "tests/expected_output/AST_strings/" + expectedOutputFileName);
    if (!expectedOutputFile.is_open())
    {
        FAIL() << "Failed to open expected output file"; // custom failure message
    }
    std::string expectedOutput((std::istreambuf_iterator<char>(expectedOutputFile)),
                               std::istreambuf_iterator<char>());
    expectedOutputFile.close();
    EXPECT_EQ(output, expectedOutput);
}

void runIRStringTest(std::string inputFileName, std::string expectedOutputFileName)
{
    //reading the input file
    std::ifstream inputFile;
    inputFile.open(relativeProjectRootPath + "examples/" + inputFileName); // open the Fortran program to translate to IR, e.g: ./main examples/summation.f90
    if (!inputFile.is_open())
    {
        FAIL() << "Failed to open input file"; // custom failure message
    }
    ANTLRInputStream input(inputFile);
    inputFile.close();

    //lexing and parsing
    Fortran90Lexer lexer(&input);
    CommonTokenStream tokens(&lexer); // create stream of tokens with lexer
    tokens.fill();                    // process all tokens before printing
    Fortran90Parser parser(&tokens);
    tree::ParseTree *parseTree = parser.program();
   
    //AST visitor
    Fortran90ParserASTVisitor astVisitor;
    antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree *>(parseTree->accept(&astVisitor)); // dispatches call to appropriate visit method in visitor

    //IR tree visitor
    EntryNode *entryNode = new EntryNode(); // will be used to store the IR tree
    Fortran90ParserIRTreeVisitor irVisitor(parser, entryNode);
    astTree->accept(&irVisitor);

    //saving the IR string
    std::string output = entryNode->stringifyIRTree();

    //comparing the output with the expected output
    std::ifstream expectedOutputFile(relativeProjectRootPath + "tests/expected_output/IR_strings/" + expectedOutputFileName);
    if (!expectedOutputFile.is_open())
    {
        FAIL() << "Failed to open expected output file"; // custom failure message
    }
    std::string expectedOutput((std::istreambuf_iterator<char>(expectedOutputFile)),
                               std::istreambuf_iterator<char>());
    expectedOutputFile.close();
    EXPECT_EQ(output, expectedOutput);
}

void runWASMTest(std::string inputFileName, std::string expectedOutputFileName)
{
        //reading the input file
    std::ifstream inputFile;
    inputFile.open(relativeProjectRootPath + "examples/" + inputFileName); // open the Fortran program to translate to IR, e.g: ./main examples/summation.f90
    if (!inputFile.is_open())
    {
        FAIL() << "Failed to open input file"; // custom failure message
    }
    ANTLRInputStream input(inputFile);
    inputFile.close();

    //lexing and parsing
    Fortran90Lexer lexer(&input);
    CommonTokenStream tokens(&lexer); // create stream of tokens with lexer
    tokens.fill();                    // process all tokens before printing
    Fortran90Parser parser(&tokens);
    tree::ParseTree *parseTree = parser.program();
   
    //AST visitor
    Fortran90ParserASTVisitor astVisitor;
    antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree *>(parseTree->accept(&astVisitor)); // dispatches call to appropriate visit method in visitor

    //IR tree visitor
    EntryNode *entryNode = new EntryNode(); // will be used to store the IR tree
    Fortran90ParserIRTreeVisitor irVisitor(parser, entryNode);
    astTree->accept(&irVisitor);

    //WASM visitor
    std::unordered_map<std::string, std::string> stringMap = irVisitor.getStringMap();
    IrWasmVisitor wasmVisitor(stringMap);
    std::string wasmOutput = wasmVisitor.getEntireProgramCode(entryNode);

    //comparing the output with the expected output 
    std::ifstream expectedOutputFile(relativeProjectRootPath + "tests/expected_output/WASM_files/" + expectedOutputFileName);
    if (!expectedOutputFile.is_open())
    {
        FAIL() << "Failed to open expected output file"; // custom failure message
    }
    std::string expectedOutput((std::istreambuf_iterator<char>(expectedOutputFile)),   //read expected output into a string
                               std::istreambuf_iterator<char>());
    expectedOutputFile.close();
    EXPECT_EQ(wasmOutput, expectedOutput);
}