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
#include "IRTreeTools.h"
#include "IrWasmVisitor.h"
#include "GhostNode.h"
#include "tree/Trees.h"
#include <regex>
#include <filesystem>
using namespace antlrcpp;
using namespace antlr4;

std::string parseTreeToDotTree(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames);


int main(int argc, const char **argv)
{
  ////////////////Setting up redirection of entire main function output to a file////////////////////////

  // std::filesystem::create_directories("output");   //creates directory if it does not exist
  // std::ofstream out("output/output.ir");
  // if (!out.is_open())
  // {
  //   std::cerr << "Failed to open output file" << std::endl;
  //   return 1;
  // }

  // std::streambuf *originalBuf = std::cout.rdbuf(); //save old buf

  // std::cout.rdbuf(out.rdbuf()); //redirect std::cout to output.ir

  ////////////////Program to translate to IR////////////////////////////////////////

  std::ifstream file;
  if (argc >= 2)
  {
    file.open(argv[1]); // open the Fortran program to translate to IR
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
  // std::string dotParseTree = parseTreeToDotTree(parseTree, parser.getRuleNames());
  // std::cout << dotParseTree << std::endl;

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
  // std::string dotASTTree = parseTreeToDotTree(astTree, parser.getRuleNames());
  // std::cout << dotASTTree << std::endl;

  /////////////////TRANSLATOR VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to translate the AST to three address code IR
  if (argc >= 3 && std::string(argv[2]) == "-irPrint")
  {
    Fortran90ParserTranslatorVisitor translatorVisitor(parser);
    astTree->accept(&translatorVisitor);
  }

  // print rule names
  // const auto& ruleNames = parser.getRuleNames();
  // std::cout << ruleNames.size() << " RULE NAMES: " << std::endl;
  // for (size_t i = 0; i < ruleNames.size(); ++i) {
  //     std::cout << ruleNames[i] << std::endl;
  // }

  ////////////////IR TREE VISITOR///////////////////////////////////////////////////

  if (argc >= 3 && (std::string(argv[2]) == "-irTree" || std::string(argv[2]) == "-irDot" || std::string(argv[2]) == "-irWASM"))
  {
    GhostNode *entryNode = new GhostNode("ENTRY");    //will be used to store the IR tree
    Fortran90ParserIRTreeVisitor irTreeVisitor(parser, entryNode);
    astTree->accept(&irTreeVisitor);

    if (std::string(argv[2]) == "-irTree")
    {
      std::cout << entryNode->stringifyIRTree() << std::endl;      //print out IR in text form, including labels

    }
    else if (std::string(argv[2]) == "-irDot")
    {
      IRTreeTools irTreeTools;
      std::string dotIRTree = irTreeTools.irTreeToDotTree(entryNode);
      std::cout << dotIRTree << std::endl;
    }
    else if (std::string(argv[2]) == "-irWASM")
    {
      std::unordered_map<std::string, std::string> stringMap = irTreeVisitor.getStringMap();
      // IRTreeTools irTreeTools;
      // std::string wasm = irTreeTools.irTreeToWASM(entryNode, stringMap);
      IrWasmVisitor wasmVisitor(stringMap);
      std::string wasm = wasmVisitor.getEntireProgramCode(entryNode);
      std::cout << wasm << std::endl;
    }
  }

  ////////////////Cleanup redirection of entire main function output to a file////////////////////////

  // std::cout.rdbuf(originalBuf); //reset to standard output again
  // out.close();

  /////////////// Write the strings from the translator visitor to a file //////////////////////////

  // std::ofstream stringFile("output/index_strings.txt");
  // if (!stringFile.is_open())
  // {
  //   std::cerr << "Failed to open string file" << std::endl;
  //   return 1;
  // }

  // //write entries to file
  // //<str_var>
  // //<string>   (on the next line)
  // std::unordered_map<std::string, std::string> stringMap = translatorVisitor.getStringMap();
  // for (const auto& pair : stringMap){
  //   stringFile << pair.first << "\n" << pair.second << std::endl;
  // }

  return 0;
}


std::string parseTreeToDotTree(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames)
{
  std::stringstream dot; // stream to store dot file
  dot << "digraph Tree {\n";
  int nodeCount = 0;
  std::unordered_map<antlr4::tree::ParseTree *, int> nodeToIndex; // map to store node and its index - used in dot file

  // Use a stack to perform DFS
  std::stack<antlr4::tree::ParseTree *> stack;
  stack.push(root);

  while (!stack.empty())
  {
    antlr4::tree::ParseTree *node = stack.top();
    stack.pop();

    // if node is not in the map (iterated to end of tree and did not find it), add it to the map and the dot file
    if (nodeToIndex.find(node) == nodeToIndex.end())
    {
      nodeToIndex[node] = nodeCount++;

      // gets text representation of node in parse tree and escapes whitespaces
      // replace " with \"
      std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, ruleNames), false);
      nodeText = std::regex_replace(nodeText, std::regex("\""), "\\\"");

      dot << "node" << nodeToIndex[node] << " [label=\"" << nodeText << "\"];\n";
    }

    // add children nodes to stack
    for (auto child : node->children)
    {

      if (nodeToIndex.find(child) == nodeToIndex.end())
      { // create node for child if not in map

        nodeToIndex[child] = nodeCount++;
        std::string childText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(child, ruleNames), false);
        childText = std::regex_replace(childText, std::regex("\""), "\\\"");
        dot << "node" << nodeToIndex[child] << " [label=\"" << childText << "\"];\n";
      }

      // add edge to dot file
      dot << "node" << nodeToIndex[node] << " -> node" << nodeToIndex[child] << ";\n";

      // add child to stack
      stack.push(child);
    }
  }

  dot << "}\n";
  return dot.str();
}


// std::string printIRTree(BaseNode *root){

// }