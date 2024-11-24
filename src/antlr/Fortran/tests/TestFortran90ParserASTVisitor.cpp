#include "MockFortran90ParserASTVisitor.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

// TODO: complete this test

// test that the visitProgram method is called when you try to use a visitor on the AST tree that has been outputted by the ASTVisitor
// the first time around,
// E.g. call ASTVisitor twice, and see if the second time, the visitProgram method should be called


//this was the code i used in main.cpp to test it ad hoc

// UNCOMMENT to create AST of the parse tree
//   Fortran90ParserASTVisitor astVisitor(parser);
//   antlr4::tree::ParseTree *astTree = std::any_cast<antlr4::tree::ParseTree*>(parseTree->accept(&astVisitor));   // dispatches call to appropriate visit method in visitor
  

//   std::cout << "AST TREE 2" << std::endl;
//   Fortran90ParserASTVisitor astVisitor2(parser);
//   antlr4::tree::ParseTree *astTree2 = std::any_cast<antlr4::tree::ParseTree*>(astTree->accept(&astVisitor2));


// And then for example, I would tag these methods in the Fortran90ParserBaseVisitor.h file, and then test that the visitProgram method is called both times

//   virtual std::any visitProgram(Fortran90Parser::ProgramContext *ctx) override {
//     std::cout << "PROGRAM: " << std::endl;
//     return visitChildren(ctx);
//   }

//   virtual std::any visitExecutableProgram(Fortran90Parser::ExecutableProgramContext *ctx) override {
//     std::cout << "EXECUTABLE PROGRAM: " << std::endl;
//     return visitChildren(ctx);
//   }

//   virtual std::any visitProgramUnit(Fortran90Parser::ProgramUnitContext *ctx) override {
//     std::cout << "PROGRAM UNIT: " << std::endl;
//     return visitChildren(ctx);
//   }


//example output
// PROGRAM: 
// EXECUTABLE PROGRAM: 
// PROGRAM UNIT: 
// AST TREE 2
// PROGRAM: 