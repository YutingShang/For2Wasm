#include "Fortran90ParserTranslatorVisitor.h"

#include "antlr4-runtime.h"
#include <cstdlib>

std::any Fortran90ParserTranslatorVisitor::visitChildren(antlr4::tree::ParseTree *node)
{

    // got rule names context from parser
    std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, this->parser.getRuleNames()), false);

    // std::cout << "Visiting LABEL: " << nodeText << std::endl;
    // return Fortran90ParserBaseVisitor::visitChildren(node);

    std::any result = defaultResult();
    size_t n = node->children.size();
    for (size_t i = 0; i < n; i++)
    {
        if (!shouldVisitNextChild(node, result))
        {
            break;
        }

        std::any childResult = node->children[i]->accept(this); // recurse for each child
        result = aggregateResult(std::move(result), std::move(childResult));
    }

    // process the current node AFTER visiting its children - POST-ORDER TRAVERSAL
    // important when overriding individual visit methods, to use `visitChildren` before processing the current node
    // std::cout << "Processed LABEL: " << nodeText << std::endl;

    return result;
}

std::any Fortran90ParserTranslatorVisitor::visitTerminal(antlr4::tree::TerminalNode *node)
{
    // std::cout << "Processed TERMINAL: " << node->getText() << std::endl;
    return Fortran90ParserBaseVisitor::visitTerminal(node);
}

std::any Fortran90ParserTranslatorVisitor::visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx)
{   
    //first visit children
    std::any result = visitChildren(ctx);

    //then process the current node
    assert(ctx->children.size() == 3);    //<NAME> <ASSIGN> <expression>

    std::string variableName = ctx->children[0]->getText();
    std::string expression = ctx->children[2]->getText();

    std::string tempVariableName = getTempVariableName(variableName);


    std::cout << "MOV " << tempVariableName << " " << expression << std::endl;

    return result;
}

std::any Fortran90ParserTranslatorVisitor::visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx){
    std::any result = visitChildren(ctx);

    // TAC - 3 address code
    // SUB a b c
    // a = b - c

    if (ctx->children[0]->getText() == "-"){
        std::string tempResultVar = getNewTempVariableName();
        std::cout << "SUB " << tempResultVar << " #0 " ;

        std::string operand = ctx->children[1]->getText();
        try{
            std::stod(operand);     //try to convert to double
            std::cout << "#" <<operand << std::endl;
        }catch(std::invalid_argument &e){
            //this means it's a variable name
            std::string tempVariableName = getTempVariableName(operand);
            std::cout << tempVariableName << std::endl;
        }

    }else if (ctx->children[0]->getText()!="+"){      //this means first token is not a + or -, so it's a variable name
        std::string tempResultVar = getNewTempVariableName();
        std::string sign = ctx->children[1]->getText();
        std::string operation = (sign == "+") ? "ADD" : "SUB";
        std::cout << operation << " " << tempResultVar << " " << ctx->children[0]->getText() << " " << ctx->children[2]->getText() << std::endl;
    }

    return result;
}

std::string Fortran90ParserTranslatorVisitor::getTempVariableName(std::string variableName){
    std::string tempVariableName;
    if (tempVariableMap.find(variableName) == tempVariableMap.end()){
        tempVariableName = "t" + std::to_string(tempVariableCount++);
        tempVariableMap[variableName] = tempVariableName;
    }else{
        tempVariableName = tempVariableMap[variableName];
    }
    return tempVariableName;
}

std::string Fortran90ParserTranslatorVisitor::getNewTempVariableName(){
    return "t" + std::to_string(tempVariableCount++);
}