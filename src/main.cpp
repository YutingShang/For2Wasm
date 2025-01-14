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
#include "PropagationOptimizer.h"
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
  if (argc >= 3 && std::string(argv[2]) == "-parseTree")
  {
    std::string dotParseTree = DotTreeTools::parseTreeToDot(parseTree, parser.getRuleNames());
    std::cout << dotParseTree << std::endl;
    return 0;     //can't have other flags after -parseTree
  }

  /////////////////CUSTOM VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to use custom visitor which prints out the nodes that would be in the AST
  // Fortran90ParserCustomVisitor visitor(parser);
  // parseTree->accept(&visitor); // dispatches call to appropriate visit method in visitor

  /////////////////AST VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to create AST of the parse tree
  Fortran90ParserASTVisitor astVisitor;
  antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree *>(parseTree->accept(&astVisitor)); // dispatches call to appropriate visit method in visitor

  // UNCOMMENT to print AST of the parse tree in LISP format
  // std::cout << astTree->toStringTree(&parser) << std::endl;

  // UNCOMMENT to print AST of the parse tree in LISP format to a file
  // std::ofstream outputFile("../tests/expected_output/AST_strings/triangularAST.txt");
  // outputFile << astTree->toStringTree(&parser);
  // outputFile.close();

  // UNCOMMENT to print AST of the parse tree in DOT format
  if (argc >= 3 && std::string(argv[2]) == "-astTree")
  {
    std::string dotASTTree = DotTreeTools::parseTreeToDot(astTree, parser.getRuleNames());
    std::cout << dotASTTree << std::endl;
    return 0;      //can't have other flags after -astTree
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

  EntryNode *entryNode = new EntryNode(); // will be used to store the IR tree
  Fortran90ParserIRTreeVisitor irTreeVisitor(parser, entryNode);
  astTree->accept(&irTreeVisitor);

  /////////////////FLOWGRAPH VISITOR///////////////////////////////////////////////////
  
  ///NOTE: may not be used by every flag

  BasicBlock *startBasicBlock = new BasicBlock();
  IrFlowgraphVisitor flowgraphVisitor(startBasicBlock);
  entryNode->accept(&flowgraphVisitor);

  /////////////////2ND, 3RD, 4TH ... FLAG///////////////////////////////////////////////////

  // .main <input file> <flag1 for output> <flag2 for optimisation> <flag3 for optimisation> ...

  for (int i = 3; i < argc; i++) {
    std::string optFlag = std::string(argv[i]);     

    //these will edit the IR tree directly - still referenced by entryNode
    if (optFlag == "-simplify") {
      SimplificationOptimisations::removeAllEmptyControlFlowConstructs(entryNode);
    }else if (optFlag == "-DCE" ) {
      DeadCodeElimination::iterateDeadCodeElimination(startBasicBlock);
    }else if (optFlag == "-CSE") {
      CSEOptimizer cseOptimizer(startBasicBlock);
      cseOptimizer.iterateCommonSubexpressionElimination();
    }else if (optFlag == "-CP") {
      PropagationOptimizer PropagationOptimizer(startBasicBlock);
      PropagationOptimizer.runCopyPropagation();
    }else if (optFlag == "-const") {
      PropagationOptimizer PropagationOptimizer(startBasicBlock);
      PropagationOptimizer.runConstantPropagation();
    }
  }

  /////////////////1ST FLAG///////////////////////////////////////////////////

  ///NOTE: now we process the first flag (the third argument at index 2) - what to output

  if (argc >=3){
    std::string flag1 = std::string(argv[2]);

    if (flag1 == "-irPrint") {
      std::cout << entryNode->stringifyIRTree() << std::endl; // print out IR in text form, including labels
    }else if (flag1 == "-irTree") {
      std::string dotIRTree = DotTreeTools::irTreeToDot(entryNode);
      std::cout << dotIRTree << std::endl;
    }else if (flag1 == "-WASM") {
      std::unordered_map<std::string, std::string> stringMap = irTreeVisitor.getStringMap();
      IrWasmVisitor wasmVisitor(stringMap);
      std::string wasm = wasmVisitor.getEntireProgramCode(entryNode);
      std::cout << wasm << std::endl;
    }else if (flag1 == "-flowgraph") {
      //redraw the flowgraph
      delete startBasicBlock;
      BasicBlock* newStartBasicBlock = new BasicBlock();
      IrFlowgraphVisitor flowgraphVisitor(newStartBasicBlock);
      entryNode->accept(&flowgraphVisitor);

      std::string dotFlowgraph = DotTreeTools::flowgraphToDot(newStartBasicBlock);
      std::cout << dotFlowgraph << std::endl;
    }
  
  }

  return 0;
}
