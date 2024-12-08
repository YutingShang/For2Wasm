#include "Fortran90ParserASTVisitor.h"

#include "antlr4-runtime.h"
#include "Fortran90ContextCopy.h"

std::any Fortran90ParserASTVisitor::visitChildren(antlr4::tree::ParseTree *node)
{

    size_t numChildren = node->children.size();
    if (!shouldVisitNextChild(node, nullptr))
    {
        return nullptr;
    }

    if (numChildren == 1)
    {
        // skip over this node, just move on to processing the next node, with this visitor's implementation

        std::any childResult = node->children[0]->accept(this);
        return childResult;
    }
    else // visit this node and its children
    {

        antlr4::ParserRuleContext *nodeToCopy = dynamic_cast<antlr4::ParserRuleContext *>(node); // cast current node to ParserRuleContext to get its metadata

        // Create new node of the same concrete type using helper function
        antlr4::ParserRuleContext *newNode = Fortran90ContextCopy::copyNode(nodeToCopy);

        // got rule names context from parser
        // std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, this->parser.getRuleNames()), false);
        // std::cout << "Visiting LABEL: " << nodeText << std::endl;

        for (size_t i = 0; i < numChildren; i++)
        {

            antlr4::tree::ParseTree *childNode = std::any_cast<antlr4::tree::ParseTree *>(node->children[i]->accept(this)); // recurse for every node child
            childNode->parent = newNode;                                                                                    // set the parent of the child node to the current node
            newNode->children.push_back(childNode);                                                                         // add the child node to the current node's children
        }

        return static_cast<antlr4::tree::ParseTree *>(newNode);
    }
}

std::any Fortran90ParserASTVisitor::visitTerminal(antlr4::tree::TerminalNode *node)
{
    // std::cout << "Visiting TERMINAL: " << node->getText() << std::endl;

    antlr4::tree::TerminalNodeImpl *newNode = new antlr4::tree::TerminalNodeImpl(node->getSymbol()); // create new node

    return static_cast<antlr4::tree::ParseTree *>(newNode);
}
