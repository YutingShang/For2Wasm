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
#include "tree/Trees.h"
#include <regex>

using namespace antlrcpp;
using namespace antlr4;

std::string toDotTree(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames);

int main(int, const char **)
{

  std::ifstream file("../../../examples/summation.f90");
  if (!file.is_open())
  {
    std::cerr << "Failed to open file" << std::endl;
    return 1;
  }

  ANTLRInputStream input(file);


  //////////////////LEXER////////////////////////////////////////////////////////

  Fortran90Lexer lexer(&input);
  CommonTokenStream tokens(&lexer); // create stream of tokens with lexer
  tokens.fill(); // process all tokens before printing

  // UNCOMMENT to print all tokens
  // for (auto token : tokens.getTokens())
  // {
  //   std::cout << token->toString() << std::endl;
  // }

  //////////////////PARSER////////////////////////////////////////////////////////

  Fortran90Parser parser(&tokens);          // create parser that works on stream of tokens
  tree::ParseTree *parseTree = parser.program(); // get first node matched by rule

  // UNCOMMENT to print parse tree in LISP format
  // std::cout << parseTree->toStringTree(&parser) << std::endl;

  // UNCOMMENT to print entire parse tree in DOT format
  // std::string dotParseTree = toDotTree(parseTree, parser.getRuleNames());
  // std::cout << dotParseTree << std::endl;

  /////////////////CUSTOM VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to use custom visitor which prints out the nodes that would be in the AST
  // Fortran90ParserCustomVisitor visitor(parser);
  // parseTree->accept(&visitor); // dispatches call to appropriate visit method in visitor
  

  /////////////////AST VISITOR///////////////////////////////////////////////////

  // UNCOMMENT to create AST of the parse tree
  Fortran90ParserASTVisitor astVisitor(parser);
  antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree*>(parseTree->accept(&astVisitor));   // dispatches call to appropriate visit method in visitor
  

  // UNCOMMENT to print AST of the parse tree in LISP format
  // std::cout << astTree->toStringTree(&parser) << std::endl;

  // UNCOMMENT to print AST of the parse tree in DOT format
  // std::string dotASTTree = toDotTree(astTree, parser.getRuleNames());
  // std::cout << dotASTTree << std::endl;
  

  /////////////////TRANSLATOR VISITOR///////////////////////////////////////////////////

  Fortran90ParserTranslatorVisitor translatorVisitor(parser);
  astTree->accept(&translatorVisitor);


  // print rule names
  // const auto& ruleNames = parser.getRuleNames();
  // std::cout << ruleNames.size() << " RULE NAMES: " << std::endl;
  // for (size_t i = 0; i < ruleNames.size(); ++i) {
  //     std::cout << ruleNames[i] << std::endl;
  // }

  return 0;
}

std::string toDotTree(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames)
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