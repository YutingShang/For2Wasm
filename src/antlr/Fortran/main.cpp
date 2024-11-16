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


using namespace antlrcpp;
using namespace antlr4;

int main(int , const char **) {
    ANTLRInputStream input(u8"program hello\n!this is a comment line - it is ignored by the compiler\n\treal :: x\n\tx = 7.0\n\tx = x/8\n\tprint *, 'Hello, World!', x\nend program hello\n");
    Fortran90Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }

  Fortran90Parser parser(&tokens);
  tree::ParseTree *tree = parser.program();

  std::cout << tree->toStringTree(&parser) << std::endl;

  return 0;
}
