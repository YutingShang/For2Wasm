#include "Fortran90ParserCustomVisitor.h"

#include "antlr4-runtime.h"

std::any Fortran90ParserCustomVisitor::visitChildren(antlr4::tree::ParseTree *node)
{

    // got rule names context from parser
    std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, this->parser.getRuleNames()), false);
    size_t numChildren = node->children.size();

    if (numChildren == 1)
    {
        // skip over this node, just call base class visitChildren implementation
        return Fortran90ParserBaseVisitor::visitChildren(node);
    }
    else
    {
        // visit this node and its children
        std::cout << "Visiting LABEL: " << nodeText << std::endl;
        return Fortran90ParserBaseVisitor::visitChildren(node);
    }
}

std::any Fortran90ParserCustomVisitor::visitTerminal(antlr4::tree::TerminalNode *node)
{
    std::cout << "Visiting TERMINAL: " << node->getText() << std::endl;
    return Fortran90ParserBaseVisitor::visitTerminal(node);
}