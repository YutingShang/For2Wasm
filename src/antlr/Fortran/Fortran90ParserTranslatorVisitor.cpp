#include "Fortran90ParserTranslatorVisitor.h"

#include "antlr4-runtime.h"
#include <cstdlib>

std::any Fortran90ParserTranslatorVisitor::visitChildren(antlr4::tree::ParseTree *node)
{

    // got rule names context from parser
    std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, this->parser.getRuleNames()), false);

    // std::cout << "Visiting LABEL: " << nodeText << std::endl;

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
    // return Fortran90ParserBaseVisitor::visitTerminal(node);
    return node->getText();
}

std::any Fortran90ParserTranslatorVisitor::visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx)
{
    //assignmentStmt will be in the form <NAME> <ASSIGN> <expression>
    assert(ctx->children.size() == 3); 

    // first visit children - recurse on the expression
    std::string variableName = ctx->children[0]->getText();
    std::string expression = std::any_cast<std::string>(ctx->children[2]->accept(this));

    //then process the current node
    std::cout << "MOV " << variableName << " " << expression << std::endl;

    return nullptr;     //QUESTION?: what to return for assignmentStmt? - maybe an instruction block label?
}

std::any Fortran90ParserTranslatorVisitor::visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx)
{
    //level2Expr will be in the form: 
    //sign? addOperand ((PLUS | MINUS) addOperand)*

    std::string lastTempVar;    //when evaluating the expression from left to right, lastTempVar stores the result of the intermediate binary operation
    bool hadSign = false;

    //assuming nodes with one child have already been pruned
    
    if (ctx->children[0]->getText() == "-"){            //check the sign, do 0-addOperand to get `-`
        std::string tempResultVar = getNewTempVariableName();
        lastTempVar = tempResultVar;
        hadSign = true;
        
        std::string addOperand1 = std::any_cast<std::string>(ctx->children[1]->accept(this));     //recurse on the addOperand

        //print out the const values in the IR without # at the front
        std::cout << "SUB " << tempResultVar << " 0 " << addOperand1 << std::endl;   
    }
    else{       //does not start with a `-`, process the first ADD or SUB
      
        std::string otherTempResultVar = getNewTempVariableName();
        lastTempVar = otherTempResultVar;
        std::string operation = ctx->children[1]->getText();
        std::string operationSymbol = (operation == "+") ? "ADD" : "SUB";
        std::string addOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
        std::string addOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
        std::cout << operationSymbol << " " << otherTempResultVar << " " << addOperand1 << " " << addOperand2 << std::endl;
    }

    //now we have the lastTempVar in both cases
    //if we had the `-` case, then the next `addOperand` will be at the index 3
    //otherwise, it will be at the index 4 (since we already handled the first sum)

    size_t index = hadSign ? 3 : 4;
    while (index < ctx->children.size()){
        std::string nextAddOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();

        std::string operation = ctx->children[index-1]->getText();
        std::string operationSymbol = (operation == "+") ? "ADD" : "SUB";
        std::cout << operationSymbol << " " << newTempVar << " " << lastTempVar << " " << nextAddOperand << std::endl;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;   //return the last temp variable the result of this level2Expr is stored in
    
}

std::any Fortran90ParserTranslatorVisitor::visitAddOperand(Fortran90Parser::AddOperandContext *ctx)
{
   
    if (ctx->children.size() == 1)
    {
        //well this case technically shouldn't happen, since all nodes with one child have already been pruned
        return ctx->children[0]->accept(this);
    }
    else
    {
        // of the form  multOperand ((STAR | DIV) multOperand)*   
        //(should have STAR or DIV more than once, since we already handled the case where there's only one multOperand)
        std::string operation = ctx->children[1]->getText();
        std::string operationSymbol = (operation == "*") ? "MUL" : "DIV";
        std::string multOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
        std::string multOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));

        std::string tempResultVar = getNewTempVariableName();
        std::string lastTempVar = tempResultVar;
        std::cout << operationSymbol << " " << tempResultVar << " " << multOperand1 << " " << multOperand2 << std::endl;

        // TODO: repeat for the next multOperand

        size_t index = 4;
        while (index < ctx->children.size()){
            std::string nextMultOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
            std::string newTempVar = getNewTempVariableName();
            std::string operation = ctx->children[index-1]->getText();
            std::string operationSymbol = (operation == "*") ? "MUL" : "DIV";
            std::cout << operationSymbol << " " << newTempVar << " " << lastTempVar << " " << nextMultOperand << std::endl;

            lastTempVar = newTempVar;
            index += 2;
        }


        return lastTempVar;
    }
}

// std::any Fortran90ParserTranslatorVisitor::visitMultOperand(Fortran90Parser::MultOperandContext *ctx){
//     if (ctx->children.size()==1){
//         return ctx->children[0]->accept(this);
//     }else{
//         // of the form level1Expr (POWER level1Expr)*
//         //Not supported in wasm, nor my IR

//     }

// }

// std::any Fortran90ParserTranslatorVisitor::visitLevel1Expr(Fortran90Parser::Level1ExprContext *ctx){

// }

// std::any Fortran90ParserTranslatorVisitor::visitDefinedUnaryOp(Fortran90Parser::DefinedUnaryOpContext *ctx){

// }

std::any Fortran90ParserTranslatorVisitor::visitPrimary(Fortran90Parser::PrimaryContext *ctx)
{
    // primary will be in the form:
    // <LPAREN> <expression> <RPAREN>

    // so we just want to return the result of visiting the expression

    return ctx->children[1]->accept(this);
}

// std::string Fortran90ParserTranslatorVisitor::getTempVariableName(std::string variableName)
// {
//     std::string tempVariableName;
//     if (tempVariableMap.find(variableName) == tempVariableMap.end())
//     {
//         tempVariableName = "t" + std::to_string(tempVariableCount++);
//         tempVariableMap[variableName] = tempVariableName;
//     }
//     else
//     {
//         tempVariableName = tempVariableMap[variableName];
//     }
//     return tempVariableName;
// }

std::string Fortran90ParserTranslatorVisitor::getNewTempVariableName()
{
    return "t" + std::to_string(tempVariableCount++);
}