#pragma once

#include "Fortran90ParserASTVisitor.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

class MockFortran90ParserASTVisitor : public Fortran90ParserASTVisitor {
public:
    MOCK_METHOD(std::any, visitAssignmentStmt, (Fortran90Parser::AssignmentStmtContext*), (override));
    MOCK_METHOD(std::any, visitProgram, (Fortran90Parser::ProgramContext*), (override));
    // Mock additional visit methods as needed for other node types

    //https://google.github.io/googletest/gmock_for_dummies.html#:~:text=As%20you%20might%20have%20guessed%2C%20this%20test%20checks%20that%20PenDown()%20is%20called%20at%20least%20once.%20If%20the%20painter%20object%20didn%E2%80%99t%20call%20this%20method%2C%20your%20test%20will%20fail%20with%20a%20message%20like%20this%3A
};