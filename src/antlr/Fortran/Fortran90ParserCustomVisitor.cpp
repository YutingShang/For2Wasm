#include "Fortran90ParserCustomVisitor.h"



std::any Fortran90ParserCustomVisitor::visitChildren(antlr4::tree::ParseTree *node) {

    std::cout << "Text from this Node: " << node->getText() << std::endl;
    std::cout << "Children number: " << node -> children.size() << std::endl;
    if (node -> children.size() == 1){
        std::cout << "Number of children of this child: " << node -> children[0] -> children.size() << std::endl;
    }
    // Continue traversing the tree by visiting children
    return Fortran90ParserBaseVisitor::visitChildren(node);
}

// std::any Fortran90ParserCustomVisitor::visitProgram(Fortran90Parser::ProgramContext *ctx) {
//     std::cout << "Visiting Program" << std::endl;
//     std::cout << ctx->getText() << std::endl;
//     return Fortran90ParserBaseVisitor::visitProgram(ctx);
// }