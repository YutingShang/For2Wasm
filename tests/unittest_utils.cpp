#include "unittest_utils.h"
#include "PropagationOptimizer.h"
#include <fstream>
#include <antlr4-runtime.h>
#include "Fortran90Lexer.h"
#include "Fortran90Parser.h"
#include "Fortran90ParserASTVisitor.h"
#include "Fortran90ParserIRTreeVisitor.h"
#include "IrFlowgraphVisitor.h"
#include "DeadCodeElimination.h"
#include "SimplificationOptimisations.h"
#include "CSEOptimizer.h"
#include "IrWasmVisitor.h"
#include "IrTypeVisitor.h"
#include "EntryNode.h"
#include "PREOptimizer.h"
#include "LoopTiling.h"

using namespace antlr4;

std::string relativeProjectRootPath = "../../";      //from where the makefile is run

void run_custom_pipeline_test(std::string inputFileName, std::string expectedOutputFileName, OutputFlag outputFlag, std::vector<OptimisationFlag> optimisationFlags) {
    //reading the input file
    std::ifstream inputFile;
    std::string inputFilePath = relativeProjectRootPath + "examples/test/" + inputFileName;
    inputFile.open(inputFilePath); // open the Fortran program to translate to IR, e.g: ./main examples/summation.f90
    if (!inputFile.is_open())
    {
        FAIL() << "Failed to open input file : " + inputFilePath; // custom failure message
    }
    ANTLRInputStream input(inputFile);
    inputFile.close();

    //reading the expected output
    std::string expectedOutputFilePath = relativeProjectRootPath + "tests/expected_output/";
    expectedOutputFilePath += (outputFlag == IR ? "IR_strings/" : outputFlag == WASM ? "WASM_files/" : "AST_strings/");
    expectedOutputFilePath += expectedOutputFileName;
    std::ifstream expectedOutputFile(expectedOutputFilePath);
    if (!expectedOutputFile.is_open())
    {
        FAIL() << "Failed to open expected output file : " + expectedOutputFilePath; // custom failure message
    }
    std::string expectedOutput((std::istreambuf_iterator<char>(expectedOutputFile)),   //read expected output into a string
                               std::istreambuf_iterator<char>());
    expectedOutputFile.close();

    //lexing and parsing
    Fortran90Lexer lexer(&input);
    CommonTokenStream tokens(&lexer); // create stream of tokens with lexer
    tokens.fill();                    // process all tokens before printing
    Fortran90Parser parser(&tokens);
    tree::ParseTree *parseTree = parser.program();
   
    //AST visitor
    Fortran90ParserASTVisitor astVisitor;
    antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree *>(parseTree->accept(&astVisitor)); // dispatches call to appropriate visit method in visitor

    std::string outputToCompare;
    if (outputFlag == AST){
        outputToCompare = astTree->toStringTree(&parser);
        EXPECT_EQ(outputToCompare, expectedOutput);
        return;
    }else{
        //IR tree visitor
        std::shared_ptr<EntryNode> entryNode = std::make_shared<EntryNode>(); // will be used to store the IR tree
        Fortran90ParserIRTreeVisitor irVisitor(parser, entryNode);
        astTree->accept(&irVisitor);

        //flowgraph is drawn in each optimisation pass

        //type visitor
        IrTypeVisitor typeVisitor;
        entryNode->accept(typeVisitor);
        std::unordered_map<std::string, std::string> irDatatypeMap = typeVisitor.getVariableIRDatatypeMap();

        //optimisations
        int nextProgramTempVariableCount = 0;
        if (optimisationFlags.size() > 0){
            for (OptimisationFlag optimisationFlag : optimisationFlags){
                if (optimisationFlag == DCE){
                    DeadCodeElimination deadCodeElimination(entryNode);
                    deadCodeElimination.iterateDeadCodeElimination();
                }
                else if (optimisationFlag == Simplify){
                    SimplificationOptimisations::removeAllEmptyControlFlowConstructs(entryNode);
                    SimplificationOptimisations::removeUnusedDeclareStatements(entryNode);
                }
                else if (optimisationFlag == CSE){
                    CSEOptimizer cseOptimizer(entryNode, irDatatypeMap, nextProgramTempVariableCount);
                    cseOptimizer.runCommonSubexpressionElimination();
                    nextProgramTempVariableCount = cseOptimizer.getNextProgramTempVariableCount();
                    irDatatypeMap = cseOptimizer.getUpdatedIRDatatypeMap();
                }
                else if (optimisationFlag == CP){
                    PropagationOptimizer PropagationOptimizer(entryNode);
                    PropagationOptimizer.runCopyPropagation();
                }
                else if (optimisationFlag == Const){
                    PropagationOptimizer PropagationOptimizer(entryNode);
                    PropagationOptimizer.runConstantPropagation();
                }
                else if (optimisationFlag == IterCSE_CP){
                    CSEOptimizer cseOptimizer(entryNode, irDatatypeMap, nextProgramTempVariableCount);
                    cseOptimizer.iterateCSE_CopyPropagation();
                    nextProgramTempVariableCount = cseOptimizer.getNextProgramTempVariableCount();
                    irDatatypeMap = cseOptimizer.getUpdatedIRDatatypeMap();
                }
                else if (optimisationFlag == PRE){
                    PREOptimizer preOptimizer(entryNode, irDatatypeMap, nextProgramTempVariableCount);
                    preOptimizer.runPartialRedundancyElimination();
                    nextProgramTempVariableCount = preOptimizer.getNextProgramTempVariableCount();
                    irDatatypeMap = preOptimizer.getUpdatedIRDatatypeMap();
                }
                else if (optimisationFlag == IterPRE_CP){
                    PREOptimizer preOptimizer(entryNode, irDatatypeMap, nextProgramTempVariableCount);
                    preOptimizer.iteratePRE_CopyPropagation();
                    nextProgramTempVariableCount = preOptimizer.getNextProgramTempVariableCount();
                    irDatatypeMap = preOptimizer.getUpdatedIRDatatypeMap();
                }
                else if (optimisationFlag == PRE_2){
                    PREOptimizer preOptimizer(entryNode, irDatatypeMap, nextProgramTempVariableCount);
                    preOptimizer.runPartialRedundancyEliminationEfficiently();
                    nextProgramTempVariableCount = preOptimizer.getNextProgramTempVariableCount();
                    irDatatypeMap = preOptimizer.getUpdatedIRDatatypeMap();
                }
                else if (optimisationFlag == IterPRE_CP_2){
                    PREOptimizer preOptimizer(entryNode, irDatatypeMap, nextProgramTempVariableCount);
                    preOptimizer.iterateEfficientPRE_CopyPropagation();
                    nextProgramTempVariableCount = preOptimizer.getNextProgramTempVariableCount();
                    irDatatypeMap = preOptimizer.getUpdatedIRDatatypeMap();
                }
                else if (optimisationFlag == Tile){
                    LoopTiling tiling(entryNode, irVisitor.getNextLoopLabelNumber(), irVisitor.getNextIfLabelNumber(), irVisitor.getNextTempVariableCount(), irDatatypeMap);
                    tiling.runLoopTiling();
                    irDatatypeMap = tiling.getUpdatedIRDatatypeMap();
                }
            }
        }

        //output format
        if (outputFlag == IR){
            //saving the IR string
            outputToCompare = entryNode->stringifyIRTree();
        }else if (outputFlag == WASM){
            //WASM visitor
            std::unordered_map<std::string, std::string> stringMap = irVisitor.getStringMap();
            IrWasmVisitor wasmVisitor(stringMap, irDatatypeMap);
            outputToCompare = wasmVisitor.getEntireProgramCode(entryNode);
        }

        //comparing the output with the expected output
        EXPECT_EQ(outputToCompare, expectedOutput);
        return;
    }
}
