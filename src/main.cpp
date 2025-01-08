/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

//
//  main.cpp
//  antlr4-cpp-demo
//
//  Created by Mike Lischke on 13.03.16.
//

#include <iostream>

#include "antlr4-runtime.h"
#include "Fortran90LexerBase.h"
#include "Fortran90Lexer.h"
#include "Fortran90Parser.h"
#include "Fortran90ParserBaseVisitor.h"
#include "Fortran90ParserCustomVisitor.h"
#include "Fortran90ParserASTVisitor.h"
#include "Fortran90ParserTranslatorVisitor.h"
#include "Fortran90ParserIRTreeVisitor.h"
#include "DotTreeTools.h"
#include "IrWasmVisitor.h"
#include "EntryNode.h"
#include "IrFlowgraphVisitor.h"
#include "DeadCodeElimination.h"
#include "CSEOptimizer.h"
#include "tree/Trees.h"
#include "SimplificationOptimisations.h"
#include <regex>
#include <filesystem>
using namespace antlrcpp;
using namespace antlr4;

int main(int argc, const char **argv)
{
  ////////////////Program to translate to IR////////////////////////////////////////

  std::ifstream file;
  if (argc >= 2)
  {
    file.open(argv[1]); // open the Fortran program to translate to IR, e.g: ./main examples/summation.f90
    if (!file.is_open())
    {
      std::cerr << "Failed to open input file" << std::endl;
      return 1;
    }
  }
  else
  {
    std::cerr << "No input file provided" << std::endl;
    return 1;
  }

  ANTLRInputStream input(file);

  //////////////////LEXER////////////////////////////////////////////////////////

  Fortran90Lexer lexer(&input);
  CommonTokenStream tokens(&lexer); // create stream of tokens with lexer
  tokens.fill();                    // process all tokens before printing

  // UNCOMMENT to print all tokens
  // for (auto token : tokens.getTokens())
  // {
  //   std::cout << token->toString() << std::endl;
  // }

  //////////////////PARSER////////////////////////////////////////////////////////

  Fortran90Parser parser(&tokens);               // create parser that works on stream of tokens
  tree::ParseTree *parseTree = parser.program(); // get first node matched by rule

  // UNCOMMENT to print parse tree in LISP format
  // std::cout << parseTree->toStringTree(&parser) << std::endl;

  // UNCOMMENT to print entire parse tree in DOT format
  if (argc >= 3 && std::string(argv[2]) == "-parseDot")
  {
    std::string dotParseTree = DotTreeTools::parseTreeToDot(parseTree, parser.getRuleNames());
    std::cout << dotParseTree << std::endl;
  }

  /////////////////CUSTOM VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to use custom visitor which prints out the nodes that would be in the AST
  // Fortran90ParserCustomVisitor visitor(parser);
  // parseTree->accept(&visitor); // dispatches call to appropriate visit method in visitor

  /////////////////AST VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to create AST of the parse tree
  Fortran90ParserASTVisitor astVisitor(parser);
  antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree *>(parseTree->accept(&astVisitor)); // dispatches call to appropriate visit method in visitor

  // UNCOMMENT to print AST of the parse tree in LISP format
  // std::cout << astTree->toStringTree(&parser) << std::endl;

  // UNCOMMENT to print AST of the parse tree in DOT format
  if (argc >= 3 && std::string(argv[2]) == "-astDot")
  {
    std::string dotASTTree = DotTreeTools::parseTreeToDot(astTree, parser.getRuleNames());
    std::cout << dotASTTree << std::endl;
  }

  /////////////////TRANSLATOR VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to translate the AST to three address code IR
  // Fortran90ParserTranslatorVisitor translatorVisitor(parser);
  // astTree->accept(&translatorVisitor);

  // print rule names
  // const auto& ruleNames = parser.getRuleNames();
  // std::cout << ruleNames.size() << " RULE NAMES: " << std::endl;
  // for (size_t i = 0; i < ruleNames.size(); ++i) {
  //     std::cout << ruleNames[i] << std::endl;
  // }

  ////////////////IR TREE VISITOR///////////////////////////////////////////////////

  /// NOTE: this is an upgraded TranslatorVisitor, instead of printing out the IR
  /// it stores the IR in a tree structure then you can print it out in different formats or convert to WASM

  if (argc >= 3 && (std::string(argv[2]) == "-irPrint" || std::string(argv[2]) == "-irDot" || std::string(argv[2]) == "-irWASM"))
  {
    EntryNode *entryNode = new EntryNode(); // will be used to store the IR tree
    Fortran90ParserIRTreeVisitor irTreeVisitor(parser, entryNode);
    astTree->accept(&irTreeVisitor);

    if (std::string(argv[2]) == "-irPrint")
    {
      std::cout << entryNode->stringifyIRTree() << std::endl; // print out IR in text form, including labels
    }
    else if (std::string(argv[2]) == "-irDot")
    {
      std::string dotIRTree = DotTreeTools::irTreeToDot(entryNode);
      std::cout << dotIRTree << std::endl;
    }
    else if (std::string(argv[2]) == "-irWASM")
    {
      std::unordered_map<std::string, std::string> stringMap = irTreeVisitor.getStringMap();

      IrWasmVisitor wasmVisitor(stringMap);
      std::string wasm = wasmVisitor.getEntireProgramCode(entryNode);
      std::cout << wasm << std::endl;
    }
  }

  /////////////////FLOWGRAPH VISITOR///////////////////////////////////////////////////

  if (argc >= 3 && (std::string(argv[2]) == "-flowgraph" || std::string(argv[2]) == "-DCE" || std::string(argv[2]) == "-DCE-ir" || std::string(argv[2]) == "-DCE-WASM" || std::string(argv[2]) == "-CSE" || std::string(argv[2]) == "-CSE-ir" || std::string(argv[2]) == "-CSE-irPrint"))
  {

    // first need to create the IR tree
    EntryNode *entryNode = new EntryNode(); // will be used to store the IR tree
    Fortran90ParserIRTreeVisitor irTreeVisitor(parser, entryNode);
    astTree->accept(&irTreeVisitor);

    // then need to create the flowgraph
    BasicBlock *startBasicBlock = new BasicBlock();
    IrFlowgraphVisitor flowgraphVisitor(startBasicBlock);
    entryNode->accept(&flowgraphVisitor);

    // then need to print the flowgraph
    if (std::string(argv[2]) == "-flowgraph")
    {
      std::string dotFlowgraph = DotTreeTools::flowgraphToDot(startBasicBlock);
      std::cout << dotFlowgraph << std::endl;
    }
    else if (std::string(argv[2]) == "-DCE")
    {
      DeadCodeElimination::iterateDeadCodeElimination(startBasicBlock); // removes dead code, modifies the flowgraph directly

      // after running DCE, redraw the flowgraph (might remove basic blocks that are empty) and print it
      /// QUESTION: do I need this- will any basic blocks be empty?
      // BasicBlock* newFlowgraphStart = new BasicBlock();
      // IrFlowgraphVisitor flowgraphVisitor(newFlowgraphStart);
      // entryNode->accept(&flowgraphVisitor);
      // std::string dotFlowgraph = DotTreeTools::flowgraphToDot(newFlowgraphStart);

      if (argc >= 4 && std::string(argv[3]) == "-simplify") {
        SimplificationOptimisations::removeAllEmptyControlFlowConstructs(entryNode);
      }
      //create the flowgraph again, since IR tree has been modified
      BasicBlock* newFlowgraphStart = new BasicBlock();
      IrFlowgraphVisitor flowgraphVisitor(newFlowgraphStart);
      entryNode->accept(&flowgraphVisitor);

      std::string dotFlowgraph = DotTreeTools::flowgraphToDot(newFlowgraphStart);
      std::cout << dotFlowgraph << std::endl;
    }
    else if (std::string(argv[2]) == "-DCE-ir")
    {
      // dce
      DeadCodeElimination::iterateDeadCodeElimination(startBasicBlock);
      // then print the IR tree
      // tree still has the entryNode
      if (argc >= 4 && std::string(argv[3]) == "-simplify") {
        SimplificationOptimisations::removeAllEmptyControlFlowConstructs(entryNode);
      }
      std::string dotIRTree = DotTreeTools::irTreeToDot(entryNode);
      std::cout << dotIRTree << std::endl;
    }
    else if (std::string(argv[2]) == "-DCE-WASM")
    {
      // dce
      DeadCodeElimination::iterateDeadCodeElimination(startBasicBlock);

      if (argc >= 4 && std::string(argv[3]) == "-simplify") {
        SimplificationOptimisations::removeAllEmptyControlFlowConstructs(entryNode);
      }
      
      // then run the wasm code
      std::unordered_map<std::string, std::string> stringMap = irTreeVisitor.getStringMap();
      IrWasmVisitor wasmVisitor(stringMap);
      std::string wasm = wasmVisitor.getEntireProgramCode(entryNode);
      std::cout << wasm << std::endl;

    }else if (std::string(argv[2]) == "-CSE") {
      CSEOptimizer cseOptimizer(irTreeVisitor.getNextTempVariableCount());
      cseOptimizer.iterateCommonSubexpressionElimination(startBasicBlock);

      //after running CSE, create the flowgraph again, since IR tree has been modified
      BasicBlock* newFlowgraphStart = new BasicBlock();
      IrFlowgraphVisitor flowgraphVisitor(newFlowgraphStart);
      entryNode->accept(&flowgraphVisitor);

      std::string dotFlowgraph = DotTreeTools::flowgraphToDot(newFlowgraphStart);
      std::cout << dotFlowgraph << std::endl;
    } else if (std::string(argv[2]) == "-CSE-ir") {
      CSEOptimizer cseOptimizer(irTreeVisitor.getNextTempVariableCount());
      cseOptimizer.iterateCommonSubexpressionElimination(startBasicBlock);

      //then print the IR tree
      std::string dotIRTree = DotTreeTools::irTreeToDot(entryNode);
      std::cout << dotIRTree << std::endl;
    } else if (std::string(argv[2]) == "-CSE-irPrint") {
      CSEOptimizer cseOptimizer(irTreeVisitor.getNextTempVariableCount());
      cseOptimizer.iterateCommonSubexpressionElimination(startBasicBlock);

      std::cout << entryNode->stringifyIRTree() << std::endl;
    }
     
  }

  return 0;
}
