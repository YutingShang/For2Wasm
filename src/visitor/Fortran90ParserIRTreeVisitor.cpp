#include "Fortran90ParserIRTreeVisitor.h"

#include "antlr4-runtime.h"
#include <cstdlib>

//need to include the other nodes here, where they are actually used, so not overshadowed by forward declarations
#include "IfNode.h"
#include "EntryNode.h"
#include "IfElseNode.h"
#include "TestNode.h"
#include "LoopNode.h"
#include "LoopCondNode.h"
#include "SimpleNode.h"
#include "LogicNotNode.h"
#include "LogicBinOpNode.h"
#include "ArithOpNode.h"
#include "RelOpNode.h"
#include "MovNode.h"
#include "EndBlockNode.h"
#include "ExitNode.h"
#include "DeclareNode.h"
#include "DeclareArrayNode.h"
#include "DataArrayNode.h"
#include "LoadEltNode.h"
#include "StoreEltNode.h"
#include "PrintNode.h"
#include "ReadNode.h"
#include "IRSemantics.h"

std::any Fortran90ParserIRTreeVisitor::visitChildren(antlr4::tree::ParseTree *node)
{

    // got rule names context from parser
    std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, this->parser.getRuleNames()), false);

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

std::any Fortran90ParserIRTreeVisitor::visitTerminal(antlr4::tree::TerminalNode *node)
{
    std::string text = node->getText();

    // Convert text to uppercase
    std::string textUPPER = text;
    std::transform(text.begin(), text.end(), textUPPER.begin(), ::toupper); // stores upper case of text in new string - 'EXIT' IR instruction, if we see 'exit' or 'EXIT' in the AST

    // check if the terminal is a reserved keyword
    std::vector<std::string> reservedTerminalKeywords = {"EXIT", "ENDIF", ".TRUE.", ".FALSE."};
    if (std::find(reservedTerminalKeywords.begin(), reservedTerminalKeywords.end(), textUPPER) != reservedTerminalKeywords.end())
    {
        std::shared_ptr<SimpleNode> instructionNode;

        if (textUPPER == "EXIT")
        {
            instructionNode = std::make_shared<ExitNode>();
        }
        else if (textUPPER == "ENDIF")
        {
            instructionNode = std::make_shared<EndBlockNode>(textUPPER);
        }
        else if (textUPPER == ".TRUE.")
        {
            text = "1";
            return text;
        }
        else if (textUPPER == ".FALSE.")
        {
            text = "0";
            return text;
        }

        previousParentNode.lock()->addChild(instructionNode);
        previousParentNode = instructionNode;

    }
    return text;
}

std::any Fortran90ParserIRTreeVisitor::visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx)
{
    // assignmentStmt will be in the form <NAME> <SFExprListRef>? <ASSIGN (aka = sign)> <expression>

    // first visit children - get the variable name
    std::string variableName = std::any_cast<std::string>(ctx->children[0]->accept(this));
    
    if (dynamic_cast<Fortran90Parser::SFExprListRefContext*>(ctx->children[1]) != nullptr){
        //if there is a <SFExprListRef> then we are accessing an element of a multi-dimensional array
        //so we need to create a StoreEltNode instead of a MovNode
        std::vector<std::string> sectionSubscriptValues = std::any_cast<std::vector<std::string>>(ctx->children[1]->accept(this));

        std::string expression = std::any_cast<std::string>(ctx->children[3]->accept(this));

        //create a StoreEltNode
        std::shared_ptr<StoreEltNode> storeEltNode = std::make_shared<StoreEltNode>(variableName, sectionSubscriptValues, expression);
        previousParentNode.lock()->addChild(storeEltNode);
        previousParentNode = storeEltNode;
        
        
    }
    else{
        //if there is no <SFExprListRef> then we are just assigning a value to a variable
        std::string expression = std::any_cast<std::string>(ctx->children[2]->accept(this));

        // then process the current node
        // create a new SimpleNode with the instruction
        std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(variableName, expression);

        // assign the previous parent node as the parent of the current node
        // in this case, the previous parent node would be the expression evaluated
        previousParentNode.lock()->addChild(movNode);

        // update the previous parent node to be the current node
        previousParentNode = movNode;
    }

    return nullptr;
    /// QUESTION: what to return for assignmentStmt? nothing if its 'void' instruction?
}

std::any Fortran90ParserIRTreeVisitor::visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx)
{
    // level2Expr will be in the form:
    // sign? addOperand ((PLUS | MINUS) addOperand)*

    std::string lastTempVar; // when evaluating the expression from left to right, lastTempVar stores the result of the intermediate binary operation
    bool hadSign = false;

    // assuming nodes with one child have already been pruned

    if (ctx->children[0]->getText() == "-")
    { // check the sign, do 0-addOperand to get `-`
        std::string tempResultVar = getNewTempVariableName();
        lastTempVar = tempResultVar;
        hadSign = true;

        std::string addOperand1 = std::any_cast<std::string>(ctx->children[1]->accept(this)); // recurse on the addOperand

        // print out the const values in the IR without # at the front
        std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>("SUB", tempResultVar, "0", addOperand1);
        previousParentNode.lock()->addChild(arithOpNode);
        previousParentNode = arithOpNode;
    }
    else
    { // does not start with a `-`, process the first ADD or SUB

        std::string otherTempResultVar = getNewTempVariableName();
        lastTempVar = otherTempResultVar;
        std::string operation = ctx->children[1]->getText();
        std::string operationSymbol = (operation == "+") ? "ADD" : "SUB";
        std::string addOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
        std::string addOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
        std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>(operationSymbol, otherTempResultVar, addOperand1, addOperand2);
        previousParentNode.lock()->addChild(arithOpNode);
        previousParentNode = arithOpNode;
    }

    // now we have the lastTempVar in both cases
    // if we had the `-` case, then the next `addOperand` will be at the index 3
    // otherwise, it will be at the index 4 (since we already handled the first sum)

    size_t index = hadSign ? 3 : 4;
    while (index < ctx->children.size())
    {
        std::string nextAddOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();

        std::string operation = ctx->children[index - 1]->getText();
        std::string operationSymbol = (operation == "+") ? "ADD" : "SUB";
        std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>(operationSymbol, newTempVar, lastTempVar, nextAddOperand);
        previousParentNode.lock()->addChild(arithOpNode);
        previousParentNode = arithOpNode;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar; // return the last temp variable the result of this level2Expr is stored in
}

std::any Fortran90ParserIRTreeVisitor::visitAddOperand(Fortran90Parser::AddOperandContext *ctx)
{

    if (ctx->children.size() == 1)
    {
        // well this case technically shouldn't happen, since all nodes with one child have already been pruned
        return ctx->children[0]->accept(this);
    }
    else
    {
        // of the form  multOperand ((STAR | DIV) multOperand)*
        //(should have STAR or DIV more than once, since we already handled the case where there's only one multOperand)

        // calculate first pair of multOperands
        std::string operation = ctx->children[1]->getText();
        std::string operationSymbol = (operation == "*") ? "MUL" : "DIV";
        std::string multOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
        std::string multOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));

        std::string tempResultVar = getNewTempVariableName();
        std::string lastTempVar = tempResultVar;
        std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>(operationSymbol, tempResultVar, multOperand1, multOperand2);
        previousParentNode.lock()->addChild(arithOpNode);
        previousParentNode = arithOpNode;

        // repeat for the next multOperand

        size_t index = 4;
        while (index < ctx->children.size())
        {
            std::string nextMultOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
            std::string newTempVar = getNewTempVariableName();
            std::string operation = ctx->children[index - 1]->getText();
            std::string operationSymbol = (operation == "*") ? "MUL" : "DIV";
            std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>(operationSymbol, newTempVar, lastTempVar, nextMultOperand);
            previousParentNode.lock()->addChild(arithOpNode);
            previousParentNode = arithOpNode;

            lastTempVar = newTempVar;
            index += 2;
        }

        return lastTempVar;
    }
}

std::any Fortran90ParserIRTreeVisitor::visitMultOperand(Fortran90Parser::MultOperandContext *ctx)
{
    // multOperand will be in the form:
    // level1Expr (POWER level1Expr)*

    //should not be the case that there is only one level1Expr (then MultOperand is not in the AST)

    //start from the RHS since it associates to the right 2**3**3 -> 2**(3**3)
    std::string exponent = std::any_cast<std::string>(ctx->children[ctx->children.size() - 1]->accept(this));
    for (int i = ctx->children.size() - 3; i >= 0; i -= 2) {      
        std::string base = std::any_cast<std::string>(ctx->children[i]->accept(this));
        std::string tempResultVar = getNewTempVariableName();
        std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>("POW", tempResultVar, base, exponent);
        previousParentNode.lock()->addChild(arithOpNode);
        previousParentNode = arithOpNode;
        exponent = tempResultVar;
    }

    return exponent;  //will be the last temp variable in the loop
}

std::any Fortran90ParserIRTreeVisitor::visitPrimary(Fortran90Parser::PrimaryContext *ctx)
{
    // primary will be in the form:
    // <LPAREN> <expression> <RPAREN>

    // so we just want to return the result of visiting the expression

    return ctx->children[1]->accept(this);
}

std::any Fortran90ParserIRTreeVisitor::visitPrintStmt(Fortran90Parser::PrintStmtContext *ctx)
{
    // get outputItemList which is the children at the 3rd index
    // outputItemList will either be <outputItemList1> or <expression>

    if (dynamic_cast<Fortran90Parser::OutputItemList1Context*>(ctx->children[3]) != nullptr)
    { // explore the outputItemList1 node

        isOutputItemListForPrintStmt = true;
        // visit the outputItemList1 node, which will handle the printing of each expression/variable as they are visited
        // returns nothing so no need to store result with any_cast
        ctx->children[3]->accept(this);

        isOutputItemListForPrintStmt = false;    //guard flag in case the outputItemList1 is used for something else other than printing
    }
    else{
        //else the outputItemList is an expression/terminal, instead of outputItemList1

        std::string resultToPrint = std::any_cast<std::string>(ctx->children[3]->accept(this));
        std::shared_ptr<PrintNode> printNode = std::make_shared<PrintNode>(getItemToPrint(resultToPrint));
        previousParentNode.lock()->addChild(printNode);
        previousParentNode = printNode;
    }

    return nullptr;
    /// QUESTION: what to return for printStmt? nullptr, like a 'void' instruction?
}

std::any Fortran90ParserIRTreeVisitor::visitOutputItemList1(Fortran90Parser::OutputItemList1Context *ctx)
{

    // outputItemList1 could have more children
    // either outputItemList1 = <expression> , <expression>
    // or outputItemList1 = <outputItemList1> , <expression>  (RECURSIVE)

    // this visit method will return a list of stuff to print out
    // either the stringIndex or the variable name etc.
    // DOES the actual printing after each expression/recursive outputItemList1 - only if we are within a PrintStmt
        //if you want to use outputItemList1 for something other than printing, then you do something else (e.g. return a vector of visit results) when the isOutputItemListForPrintStmt flag is false

    if (dynamic_cast<Fortran90Parser::OutputItemList1Context*>(ctx->children[0]) != nullptr)
    {  // explore the outputItemList1 node
        //case of <outputItemList1> <expression>, process the outputItemList1
        ctx->children[0]->accept(this);
    }
    else{
        // so must the case of <expression> <expression>, process the first expression

        std::string leftChildExpr = std::any_cast<std::string>(ctx->children[0]->accept(this));
        
        if (isOutputItemListForPrintStmt){
            //Immediately add a printNode after each result
            //E.g. if the expression to print was written in the PrintStmt itself, e.g. PRINT *, 3+4, then the result is a temporary so we print immediately to consume that temp
            std::shared_ptr<PrintNode> printNode = std::make_shared<PrintNode>(getItemToPrint(leftChildExpr)); 
            previousParentNode.lock()->addChild(printNode);
            previousParentNode = printNode;
        }
    }

    // right child (at index 2 after the comma) should always be an expression to print
    //process the second expression in both cases
    std::string rightChildExpr = std::any_cast<std::string>(ctx->children[2]->accept(this));
    
    if (isOutputItemListForPrintStmt){
        std::shared_ptr<PrintNode> printNode = std::make_shared<PrintNode>(getItemToPrint(rightChildExpr)); 
        previousParentNode.lock()->addChild(printNode);
        previousParentNode = printNode;
    }

    return nullptr;  //could return an outputList if you want to use outputItemList1 for something other than printing
}

std::any Fortran90ParserIRTreeVisitor::visitReadStmt(Fortran90Parser::ReadStmtContext *ctx)
{
    // readStmt will be in the form
    // or <READ> <rdFmtId> <commaInputItemList>, e.g. READ *, a, b
    //<READ> <rdCtlSpec> <inputItemList>, e.g. READ (*,*) a

    /// NOTE: I am ignoring the format specifier for now <rdFmtId> and <rdCtlSpec>

    // In the <READ> <rdCtlSpec> <inputItemList> case:
    // if just reading 1 variable, then no inputItemList, just a single variable name
    if (ctx->children[2]->children.size() == 0)
    {
        std::string inputVar = ctx->children[2]->getText();
        std::shared_ptr<ReadNode> readNode = std::make_shared<ReadNode>(inputVar);
        previousParentNode.lock()->addChild(readNode);
        previousParentNode = readNode;
    }
    else
    {
        // get the child at index 2, which is either commaInputItemList or inputItemList
        std::vector<std::string> inputItemList = std::any_cast<std::vector<std::string>>(ctx->children[2]->accept(this));

        for (std::string item : inputItemList)
        {
            std::shared_ptr<ReadNode> readNode = std::make_shared<ReadNode>(item);
            previousParentNode.lock()->addChild(readNode);
            previousParentNode = readNode;
        }
    }

    return nullptr;

    // we want to return the list of variables to read into
}

std::any Fortran90ParserIRTreeVisitor::visitCommaInputItemList(Fortran90Parser::CommaInputItemListContext *ctx)
{
    // just ignore the comma and return the inputItemList

    // if there is only 1 variable, then no InputItemList, just a single variable name
    if (ctx->children[1]->children.size() == 0)
    {
        return std::vector<std::string>{ctx->children[1]->getText()};
    }

    return std::any_cast<std::vector<std::string>>(ctx->children[1]->accept(this));
}

std::any Fortran90ParserIRTreeVisitor::visitInputItemList(Fortran90Parser::InputItemListContext *ctx)
{
    // return the list of input variables

    std::vector<std::string> inputList;

    for (size_t i = 0; i < ctx->children.size(); i += 2)
    {
        inputList.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this)));
    }

    return inputList;
}

std::any Fortran90ParserIRTreeVisitor::visitTypeDeclarationStmt(Fortran90Parser::TypeDeclarationStmtContext *ctx)
{
    // typeDeclarationStmt will be in the form:
    // <typeSpec> <entityDeclList>
    // or <typeSpec> <attrSpec>? :: <entityDeclList>

    /// TODO: need to handle arrays here?
    ///ignoring attrSpec for now
    //the typeSpec will be at the first index, and is shared by all the variables in the entityDeclList
    //the typeSpec may be INTEGER, REAL, DOUBLE, LOGICAL, CHARACTER or INTEGER(8) etc. with a (kindSelector)!

    // entityDeclList will be a list of entityDecl separated by commas
    // or just a single variable to declare and no entityDeclList

    //get the data TYPE - could be just the INTEGER, REAL etc. or a typeSpec with a kindSelector
    std::string datatype;
    if (dynamic_cast<Fortran90Parser::TypeSpecContext*>(ctx->children[0]) != nullptr){  //process the typeSpec with a kindSelector
        datatype = std::any_cast<std::string>(ctx->children[0]->accept(this));
    }else{
        //just a simple datatype like INTEGER, REAL etc.
        datatype = getDatatype(ctx->children[0]->getText());
    }

    size_t numChildren = ctx->children.size();

    // entityDecList will always be at the last index in the children vector
    int entityDeclListIndex = numChildren - 1;

    // first check if entityDeclList is actually just a single variable
    // in that case, do not recurse
    if (ctx->children[entityDeclListIndex]->children.size() == 0)
    {

        std::string variable = ctx->children[entityDeclListIndex]->getText();
        std::shared_ptr<DeclareNode> declareNode = std::make_shared<DeclareNode>(datatype, variable);
        previousParentNode.lock()->addChild(declareNode);
        previousParentNode = declareNode;
    }
    else if (dynamic_cast<Fortran90Parser::EntityDeclListContext*>(ctx->children[entityDeclListIndex]) != nullptr)
    {
        // recurse on the entityDeclList
        std::pair<std::vector<std::string>, std::vector<std::pair<std::string, std::vector<std::string>>>> entityDeclList = std::any_cast<std::pair<std::vector<std::string>, std::vector<std::pair<std::string, std::vector<std::string>>>>>(ctx->children[entityDeclListIndex]->accept(this));
        std::vector<std::string> variables = entityDeclList.first;     //normal variables
        std::vector<std::pair<std::string, std::vector<std::string>>> arrayVariables = entityDeclList.second;    //array variables, each is a pair of (arrayName, dimensions)
        
        //process the normal variables
        for (std::string variable : variables)
        {
            std::shared_ptr<DeclareNode> declareNode = std::make_shared<DeclareNode>(datatype, variable);
            previousParentNode.lock()->addChild(declareNode);
            previousParentNode = declareNode;
        }

        //process the array variables
        for (std::pair<std::string, std::vector<std::string>> arrayVariable : arrayVariables)
        {
            std::shared_ptr<DeclareArrayNode> declareArrayNode = std::make_shared<DeclareArrayNode>(datatype, arrayVariable.first, arrayVariable.second);
            previousParentNode.lock()->addChild(declareArrayNode);
            previousParentNode = declareArrayNode;
        }
    } else if (dynamic_cast<Fortran90Parser::EntityDeclContext*>(ctx->children[entityDeclListIndex]) != nullptr) {
        //process the entityDecl - would be a single array declaration
        std::pair<std::string, std::vector<std::string>> entityDecl = std::any_cast<std::pair<std::string, std::vector<std::string>>>(ctx->children[entityDeclListIndex]->accept(this));
        std::shared_ptr<DeclareArrayNode> declareArrayNode = std::make_shared<DeclareArrayNode>(datatype, entityDecl.first, entityDecl.second);
        previousParentNode.lock()->addChild(declareArrayNode);
        previousParentNode = declareArrayNode;
    }

    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitTypeSpec(Fortran90Parser::TypeSpecContext *ctx)
{
    //if there is a typeSpec node in the AST, then it is not a simple data type like INTEGER, REAL, LOGICAL etc.
    //it will be something like INTEGER(8) or REAL(4) etc.
    //so we need to return the datatype plus the kindSelector

    std::string datatype = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string kindSelectorByteSize = std::any_cast<std::string>(ctx->children[1]->accept(this));
    return getDatatype(datatype, std::stoi(kindSelectorByteSize));
}

std::any Fortran90ParserIRTreeVisitor::visitKindSelector(Fortran90Parser::KindSelectorContext *ctx)
{
    //kindSelector be like LPAREN <byteSize> RPAREN
    std::string byteSize = std::any_cast<std::string>(ctx->children[1]->accept(this));
    return byteSize;
}

std::any Fortran90ParserIRTreeVisitor::visitUnsignedArithmeticConstant(Fortran90Parser::UnsignedArithmeticConstantContext *ctx)
{
    //unsignedArithmeticConstant will be in the form:
    //<int/float CONSTANT> UNDERSCORE <kindSelector>

    //just ignore the kindSelector and return the constant
    return std::any_cast<std::string>(ctx->children[0]->accept(this));
}



std::any Fortran90ParserIRTreeVisitor::visitEntityDeclList(Fortran90Parser::EntityDeclListContext *ctx)
{
    // entityDeclList will be a list of <entityDecl> separated by commas
    /// NOTE: this is assuming <entityDecl> is just <objectName> - no ASSIGN (initialization of variables during declaration) etc.
    /// TODO: handle more complex entityDecl (including initialisation etc. if WASM allows it)

    std::vector<std::string> variables;
    std::vector<std::pair<std::string, std::vector<std::string>>> arrayVariables;    //pair of array name and dimension vector

    for (size_t i = 0; i < ctx->children.size(); i += 2)   //every 2 children is an entityDecl
    {
        // for every <entityDecl> child of entityDeclList, if it is only <objectName>, then we can just add it to the variables vector
        if(ctx->children[i]->children.size() == 0){ // if it does not have any children
            variables.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this))); // this will reach a terminal node and return the variable name
        }
        else{
            //it is an array (or other more complex entityDecl)
            ///ASSUME: array for now is the only complex entityDecl
            std::pair<std::string, std::vector<std::string>> arrayDecl = std::any_cast<std::pair<std::string, std::vector<std::string>>>(ctx->children[i]->accept(this));
            arrayVariables.push_back(arrayDecl);
        }
    }
    ///TODO: handle entityDecl if it is an array

    return std::make_pair(variables, arrayVariables);   //returns a list of both variables and arrayVariables
}

std::any Fortran90ParserIRTreeVisitor::visitEntityDecl(Fortran90Parser::EntityDeclContext *ctx)
{
    //entityDecl will be in the form:
    //<objectName> ( <explicitShapeSpecList> )
    //e.g. i(5) or j(1, 2, 3)

    //the <explicitShapeSpecList> might be a single upper bound number, or a list of upper bounds separated by commas
    //e.g. (5) or (1, 2, 3)
    //we can just return the list of numbers as a vector of strings

    //should return a pair of (variable, vector of dimensions)
    std::string variable = ctx->children[0]->getText();
    std::vector<std::string> dimensions;

    if (dynamic_cast<Fortran90Parser::ExplicitShapeSpecListContext*>(ctx->children[2]) != nullptr){
        dimensions = std::any_cast<std::vector<std::string>>(ctx->children[2]->accept(this));
    }
    else{
        dimensions.push_back(ctx->children[2]->getText());
    }

    return std::make_pair(variable, dimensions);
}

std::any Fortran90ParserIRTreeVisitor::visitExplicitShapeSpecList(Fortran90Parser::ExplicitShapeSpecListContext *ctx)
{
    //explicitShapeSpecList will be in the form:
    //<upperBound> (COMMA <upperBound>)*

    std::vector<std::string> upperBounds;

    for (size_t i = 0; i < ctx->children.size(); i += 2)
    {
        upperBounds.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this)));
    }

    return upperBounds;
    
}

std::any Fortran90ParserIRTreeVisitor::visitEquivOperand(Fortran90Parser::EquivOperandContext *ctx)
{
    // of the form <orOperand> (.OR. <orOperand>)*
    // will return a temp variable containing the result of the OR operation

    assert(ctx->children.size() >= 3); // should have at least two orOperands

    // calculate first pair of orOperands
    std::string orOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string orOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
    std::string tempResultVar = getNewTempVariableName();
    std::string lastTempVar = tempResultVar;
    std::shared_ptr<LogicBinOpNode> orNode = std::make_shared<LogicBinOpNode>("OR", tempResultVar, orOperand1, orOperand2);
    previousParentNode.lock()->addChild(orNode);
    previousParentNode = orNode;

    // repeat for the next multOperand
    size_t index = 4;
    while (index < ctx->children.size())
    {
        std::string nextOrOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();
        std::shared_ptr<LogicBinOpNode> orNode = std::make_shared<LogicBinOpNode>("OR", newTempVar, lastTempVar, nextOrOperand);
        previousParentNode.lock()->addChild(orNode);
        previousParentNode = orNode;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;
}

std::any Fortran90ParserIRTreeVisitor::visitOrOperand(Fortran90Parser::OrOperandContext *ctx)
{
    // of the form <andOperand> (.AND. <andOperand>)*

    assert(ctx->children.size() >= 3); // should have at least two orOperands

    // calculate first pair of andOperands
    std::string andOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string andOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
    std::string tempResultVar = getNewTempVariableName();
    std::string lastTempVar = tempResultVar;
    std::shared_ptr<LogicBinOpNode> andNode = std::make_shared<LogicBinOpNode>("AND", tempResultVar, andOperand1, andOperand2);
    previousParentNode.lock()->addChild(andNode);
    previousParentNode = andNode;

    // repeat for the next andOperand
    size_t index = 4;
    while (index < ctx->children.size())
    {
        std::string nextAndOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();
        std::shared_ptr<LogicBinOpNode> andNode = std::make_shared<LogicBinOpNode>("AND", newTempVar, lastTempVar, nextAndOperand);
        previousParentNode.lock()->addChild(andNode);
        previousParentNode = andNode;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;
}

std::any Fortran90ParserIRTreeVisitor::visitAndOperand(Fortran90Parser::AndOperandContext *ctx)
{
    // of the form
    // LNOT <level4Expr>  (should have the NOT, if visit this node, otherwise go directly to level4Expr)

    assert(ctx->children.size() == 2); // should have the NOT and the level4Expr

    // will output
    // NOT <not_expr> <expr>

    std::string expr = std::any_cast<std::string>(ctx->children[1]->accept(this));
    std::string tempVar = getNewTempVariableName();
    std::shared_ptr<LogicNotNode> logicNotNode = std::make_shared<LogicNotNode>(tempVar, expr);
    previousParentNode.lock()->addChild(logicNotNode);
    previousParentNode = logicNotNode;

    return tempVar;
}

std::any Fortran90ParserIRTreeVisitor::visitLevel4Expr(Fortran90Parser::Level4ExprContext *ctx)
{
    // of the form
    //  <level3Expr> (<REL_OP> <level3Expr>)*
    //  where REL_OP is relational operators .LT., .LE., .EQ., .NE., .GE., .GT.
    //  well the level3Expr will just be level2Expr in most cases

    assert(ctx->children.size() >= 3); // should have at least two level3Exprs

    // calculate first pair of level3Exprs
    std::string operation = ctx->children[1]->getText();
    std::string operationSymbol = getRelationalOperator(operation);
    std::string level3Expr1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string level3Expr2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
    std::string tempResultVar = getNewTempVariableName();
    std::string lastTempVar = tempResultVar;
    std::shared_ptr<RelOpNode> relOpNode = std::make_shared<RelOpNode>(operationSymbol, tempResultVar, level3Expr1, level3Expr2);
    previousParentNode.lock()->addChild(relOpNode);
    previousParentNode = relOpNode;

    // repeat for the next level3Expr
    size_t index = 4;
    while (index < ctx->children.size())
    {

        std::string nextLevel3Expr = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();

        std::string operation = ctx->children[index - 1]->getText();
        std::string operationSymbol = getRelationalOperator(operation);
        std::shared_ptr<RelOpNode> relOpNode = std::make_shared<RelOpNode>(operationSymbol, newTempVar, lastTempVar, nextLevel3Expr);
        previousParentNode.lock()->addChild(relOpNode);
        previousParentNode = relOpNode;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;
}

std::any Fortran90ParserIRTreeVisitor::visitIfConstruct(Fortran90Parser::IfConstructContext *ctx)
{
    // <ifThenStmt> <conditionalBody> <elseIfConstruct>* <elseConstruct>? <endIfStmt>

    // process the ifThenStmt

    /// NOTE: ignoring <elseIfConstruct> for now
    if (ctx->children.size() == 3)
    { // just an <ifThenStmt> <conditionalBody> <endIfStmt>
        std::string labelNumber = std::to_string(ifCount++);

        std::shared_ptr<IfNode> ifNode = std::make_shared<IfNode>(labelNumber);
        previousParentNode.lock()->addChild(ifNode);
        previousParentNode = ifNode;

        // process the condition block, will create its own nodes
        // no label is added to the IR nodes - these are added when printing out the IR tree
        ctx->children[0]->accept(this);
        previousParentNode = ifNode;      //set the previousParentNode back to the ifNode

        // visit the conditionalBody at index 1, if that node is a terminal, just output it (e.g. EXIT)
        /// NOTE: visitTerminal will implement printing the EXIT statement
        // otherwise conditionalBody can contain multiple <executionPartConstruct> it will visitChildren
        // or conditionalBody will contain some other (non-terminal) statement (e.g. print) it will visit
        ctx->children[1]->accept(this);
        std::shared_ptr<EndBlockNode> endThenNode = std::make_shared<EndBlockNode>("ENDTHEN");
        previousParentNode.lock()->addChild(endThenNode);
        previousParentNode = ifNode;

        // process endIfStmt - would just print "ENDIF"
        ctx->children[2]->accept(this);

        //the ENDIF would be the previousParentNode, and continue sequentially from there
    }
    else if (dynamic_cast<Fortran90Parser::ElseConstructContext*>(ctx->children[2]))
    { // <ifThenStmt> <conditionalBody> <elseConstruct> <endIfStmt>
        std::string labelNumber = std::to_string(ifCount++);   // increment the ifCount
      
        std::shared_ptr<IfElseNode> ifNode = std::make_shared<IfElseNode>(labelNumber);
        previousParentNode.lock()->addChild(ifNode);
        previousParentNode = ifNode;

        // process conditional block
        ctx->children[0]->accept(this);
        previousParentNode = ifNode;      //set the previousParentNode back to the ifNode

        // process then block
        ctx->children[1]->accept(this);
        std::shared_ptr<EndBlockNode> endThenNode = std::make_shared<EndBlockNode>("ENDTHEN");
        previousParentNode.lock()->addChild(endThenNode);
        previousParentNode = ifNode;

        // process elseConstruct
        ctx->children[2]->accept(this); // a elseConstruct
        std::shared_ptr<EndBlockNode> endElseNode = std::make_shared<EndBlockNode>("ENDELSE");
        previousParentNode.lock()->addChild(endElseNode);
        previousParentNode = ifNode;

        // process endIfStmt
        ctx->children[3]->accept(this);

        //the ENDIF would be the previousParentNode, and continue sequentially from there
    }else if (dynamic_cast<Fortran90Parser::ElseIfConstructContext*>(ctx->children[2])){
        // <ifThenStmt> <conditionalBody> <elseIfConstruct>* <elseConstruct>? <endIfStmt>
        ///NOTE: may or may not have an <elseConstruct>
        ///CONVERTING: elseif into else { if ... endif} endif but need keep count of how many endif to add at the end
        
        std::string labelNumber = std::to_string(ifCount++);
        std::shared_ptr<IfElseNode> ifElseNode = std::make_shared<IfElseNode>(labelNumber);    //create an IfElseNode
        previousParentNode.lock()->addChild(ifElseNode);
        previousParentNode = ifElseNode;

        // process the ifThenStmt 
        ctx->children[0]->accept(this);         //add the <cond> to the ifElseNode
        previousParentNode = ifElseNode;

        // process the conditionalBody
        ctx->children[1]->accept(this);         //add the <then> to the ifElseNode
        std::shared_ptr<EndBlockNode> endThenNode = std::make_shared<EndBlockNode>("ENDTHEN");
        previousParentNode.lock()->addChild(endThenNode);
        previousParentNode = ifElseNode;

        //could be multiple elseIfConstructs, so need to process each one
        ///NOTE: this will actually be the 'ELSE' body of the original ifElseNode created. But inside that we would need to create an additional ifElseNode for the 'elseif'. If there is no final 'elseConstruct' last elseIf should be an IfNode, not an IfElseNode
        int index = 2;
        std::vector<std::shared_ptr<IfElseNode>> savedIfElseNodesToAddEndBlocksTo;
        while (index < ctx->children.size() && dynamic_cast<Fortran90Parser::ElseIfConstructContext*>(ctx->children[index])){
            //check to see if the elseIfConstruct is the last child of the ifConstruct before the endIfStmt, if so, there is no elseConstruct, so create an IfNode (not an IfElseNode)
            if (index == ctx->children.size() - 2){   
                createIfElseNode = false;
            }else{
                createIfElseNode = true;
            }
            ctx->children[index]->accept(this);   //this will create the nodes with <cond> and <then>, next iteration will add the <else>, at the end of all the nests, need to add the <endif> and <endelse>
            if (dynamic_cast<IfElseNode*>(previousParentNode.lock().get())){    //if it is an IfElseNode, we need to add <endif> and <endelse> later
                savedIfElseNodesToAddEndBlocksTo.push_back(std::dynamic_pointer_cast<IfElseNode>(previousParentNode.lock()));
            }

            index ++;
        }
        int numElseIfs = index - 2;

        //process the elseConstruct if it exists - add to the very last 'else' of the innermost ifElseNode created from all the elifs
        if (index < ctx->children.size() && dynamic_cast<Fortran90Parser::ElseConstructContext*>(ctx->children[index])){
            ctx->children[index]->accept(this);
            std::shared_ptr<EndBlockNode> endElseNode = std::make_shared<EndBlockNode>("ENDELSE");
            previousParentNode.lock()->addChild(endElseNode);
            index ++;
        }
        previousParentNode = ifElseNode;   //back to the outermost original ifElseNode


        //process the endIfStmt (at the last index now) (just add 1 - should handle the other endifs at each other layer)
        ctx->children[index]->accept(this);    //adds the ENDIF to the outermost original ifElseNode

        //add the end blocks to the savedIfElseNodesToAddEndBlocksTo (like the inner nested ifElseNodes)
        for (auto ifElseNode : savedIfElseNodesToAddEndBlocksTo){
            std::shared_ptr<EndBlockNode> endIfNode = std::make_shared<EndBlockNode>("ENDIF");
            ifElseNode->addChild(endIfNode);
            std::shared_ptr<EndBlockNode> endElseNode = std::make_shared<EndBlockNode>("ENDELSE");
            endIfNode->addChild(endElseNode);
        }
        
    }

    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitIfThenStmt(Fortran90Parser::IfThenStmtContext *ctx)
{
    // IF LPAREN expression RPAREN THEN

    // process the <expression> for the condition - recurse on the expression
    // would print the block of condition code
    /// NOTE: this would also return the temp var which is the actual condition
    // I can use this temp var by printing 'TEST <var>'
    std::string conditionVar = std::any_cast<std::string>(ctx->children[2]->accept(this));

    // TEST <var>  - not actually used in conversion to WASM
    std::shared_ptr<TestNode> testNode = std::make_shared<TestNode>(conditionVar);
    previousParentNode.lock()->addChild(testNode);
    previousParentNode = testNode;

    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitElseIfConstruct(Fortran90Parser::ElseIfConstructContext *ctx)
{
    //elseIfConstruct: <elseIfStmt> <conditionalBody>
    //process the elseIfStmt


    //so depending on the createIfElseNode flag, we will create an IfElseNode or an IfNode
    std::string labelNumber = std::to_string(ifCount++);
    if (createIfElseNode){
        std::shared_ptr<IfElseNode> ifElseNode = std::make_shared<IfElseNode>(labelNumber);
        previousParentNode.lock()->addChild(ifElseNode);
        previousParentNode = ifElseNode;

        //process the elseIfStmt  (kind of like an ifThenStmt, basically just a <cond>)
        ctx->children[0]->accept(this);
        previousParentNode = ifElseNode;

        //process the conditionalBody (kind of like a <then>)
        ctx->children[1]->accept(this);
        std::shared_ptr<EndBlockNode> endThenNode = std::make_shared<EndBlockNode>("ENDTHEN");
        previousParentNode.lock()->addChild(endThenNode);
        previousParentNode = ifElseNode;    //for the next elseIfConstruct or elseConstruct

    }else{
        std::shared_ptr<IfNode> ifNode = std::make_shared<IfNode>(labelNumber);
        previousParentNode.lock()->addChild(ifNode);
        previousParentNode = ifNode;

        //process the elseIfStmt  (kind of like an ifThenStmt, basically just a <cond>)
        ctx->children[0]->accept(this);
        previousParentNode = ifNode;

        //process the conditionalBody (kind of like a <then>)
        ctx->children[1]->accept(this);
        std::shared_ptr<EndBlockNode> endThenNode = std::make_shared<EndBlockNode>("ENDTHEN");
        previousParentNode.lock()->addChild(endThenNode);
        previousParentNode = ifNode;     

        //no more elseIfConstructs, so add the endIf node and endElse node
        std::shared_ptr<EndBlockNode> endIfNode = std::make_shared<EndBlockNode>("ENDIF");
        previousParentNode.lock()->addChild(endIfNode);
        std::shared_ptr<EndBlockNode> endElseNode = std::make_shared<EndBlockNode>("ENDELSE");
        endIfNode->addChild(endElseNode);
        // previousParentNode = ifNode;
    }

    return nullptr;

}

std::any Fortran90ParserIRTreeVisitor::visitElseIfStmt(Fortran90Parser::ElseIfStmtContext *ctx)
{
    //elseIfStmt: ELSE IF LPAREN expression RPAREN THEN
            //or  ELSEIF LPAREN expression RPAREN THEN
    //so just visit the expression

    //kind of like an ifThenStmt, basically just a <cond>

    int index = ctx->children.size()-3;     //find the expression from the end of the list
    std::string conditionVar = std::any_cast<std::string>(ctx->children[index]->accept(this));
    std::shared_ptr<TestNode> testNode = std::make_shared<TestNode>(conditionVar);
    previousParentNode.lock()->addChild(testNode);
    // previousParentNode = testNode;    //probably not needed
    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitElseConstruct(Fortran90Parser::ElseConstructContext *ctx)
{
    //<elseStmt> <conditionalBody>
    /// NOTE: elseStmt will just be ELSE - already printed in visitIfConstruct
    //just visit the conditionalBody

    ctx->children[1]->accept(this);
    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitEndIfStmt(Fortran90Parser::EndIfStmtContext *ctx)
{
    std::shared_ptr<EndBlockNode> endIfNode = std::make_shared<EndBlockNode>("ENDIF");
    previousParentNode.lock()->addChild(endIfNode);
    previousParentNode = endIfNode;
    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitBlockDoConstruct(Fortran90Parser::BlockDoConstructContext *ctx){
    //of the form 
    //<nameColon>? DO <commaLoopControl>? <executionPartConstruct>* <endDoStmt>
    // if it is a DO loop with no control - requires EXIT
    ///NOTE: also ignoring <nameColon> for now - just doing a simple DO loop with no name

    // so the form would be DO <executionPartConstruct>* <endDoStmt> or DO <commaLoopControl>? <executionPartConstruct>* <endDoStmt>

   

    //process the child at index 1 - might be commaLoopControl
    if (dynamic_cast<Fortran90Parser::CommaLoopControlContext*>(ctx->children[1]) || dynamic_cast<Fortran90Parser::LoopControlContext*>(ctx->children[1])){

        //create a LoopCondNode with the loopControl
        std::string labelNumber = std::to_string(loopCount++);

        //create the new LoopCondNode first, attach to the previousParentNode
        std::shared_ptr<LoopCondNode> loopNode = std::make_shared<LoopCondNode>(labelNumber);
        previousParentNode.lock()->addChild(loopNode);
        previousParentNode = loopNode;

        //process the commaLoopControl or loopControl, add as first child of the loopNode
        //but capture the loopControl struct, so I can break away the connections between the initialisationNode, condition block and the step block
        doLoopStruct loopControl = std::any_cast<doLoopStruct>(ctx->children[1]->accept(this));
        loopControl.initialisationEndNode->removeChild(*loopControl.condTopNode);
        loopControl.condEndNode->removeChild(*loopControl.stepTopNode);

        //add the condition as the second child of the loopNode
        loopNode->addChild(loopControl.condTopNode);    

        //add the body to the loopNode as the third child, by processing the body at index 2 of the BlockDoConstruct
        previousParentNode = loopNode;
        for(size_t i = 2; i < ctx->children.size() - 1; i++){      //process each executionPartConstruct into sequential nodes
            ctx->children[i]->accept(this);
        }
        std::shared_ptr<EndBlockNode> endBodyNode = std::make_shared<EndBlockNode>("ENDBODY");    //add an ENDBODY at the end of the loop body
        previousParentNode.lock()->addChild(endBodyNode);
        //add the step block to the loopNode as the fourth child
        loopNode->addChild(loopControl.stepTopNode);

        //reset the previousParentNode to the loopNode
        previousParentNode = loopNode;

    }else{

        //create a normal LoopNode
        std::string labelNumber = std::to_string(loopCount++);

        std::shared_ptr<LoopNode> loopNode = std::make_shared<LoopNode>(labelNumber);
        previousParentNode.lock()->addChild(loopNode);
        previousParentNode = loopNode;

        // process the body of the loop, either starting at index 1 
        for(size_t i = 1; i < ctx->children.size() - 1; i++){      //process each executionPartConstruct into sequential nodes
            ctx->children[i]->accept(this);
        }
        std::shared_ptr<EndBlockNode> endBodyNode = std::make_shared<EndBlockNode>("ENDBODY");    //add an ENDBODY at the end of the loop body
        previousParentNode.lock()->addChild(endBodyNode);

        previousParentNode = loopNode;
    }

    //add the ENDLOOP at the end of the loop
    std::shared_ptr<EndBlockNode> endLoopNode = std::make_shared<EndBlockNode>("ENDLOOP");
    previousParentNode.lock()->addChild(endLoopNode);
    previousParentNode = endLoopNode;

    return nullptr;

}

std::any Fortran90ParserIRTreeVisitor::visitCommaLoopControl(Fortran90Parser::CommaLoopControlContext *ctx) {
    //COMMA? loopControl
    

    if (ctx->children.size() == 2) {
        //ignore the comma and just visit the loopControl, return the doLoopStruct
        return ctx->children[1]->accept(this);
    }
    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitLoopControl(Fortran90Parser::LoopControlContext *ctx) {
    //loopControl
    //of the form: variableName ASSIGN expression COMMA expression commaExpr?
    ///NOTE: the following should probably all be integers:
    //the first expression is the initial value of the loop variable
    //the second expression is the terminating value of the loop variable
    //where commaExpr is the 'step' expression

    //create a struct to store the loopControl, which we will return to the BlockDoConstruct
    doLoopStruct loopControl;

    //process the variableName and the initialisation expression
    std::string variableName = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string initialisationExpression = std::any_cast<std::string>(ctx->children[2]->accept(this));

    //create a MovNode to initialise the loop variable
    std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(variableName, initialisationExpression);
    previousParentNode.lock()->addChild(movNode);
    previousParentNode = movNode;

    //store the MovNode as the initialisationEndNode
    loopControl.initialisationEndNode = movNode;    

    //process the terminating expression, create a TestNode
    std::string terminatingExpression = std::any_cast<std::string>(ctx->children[4]->accept(this));
    //create a GT RelOpNode, and a TestNode
    //since the terminating condition for the do loop is when loop variable > terminatingExpression
    std::string tempBoolVar = getNewTempVariableName();
    std::shared_ptr<RelOpNode> relOpNode = std::make_shared<RelOpNode>("GT", tempBoolVar, variableName, terminatingExpression);
    previousParentNode.lock()->addChild(relOpNode);
    previousParentNode = relOpNode;
    std::shared_ptr<TestNode> testNode = std::make_shared<TestNode>(tempBoolVar);
    previousParentNode.lock()->addChild(testNode);
    previousParentNode = testNode;

    //whichever is the child of the movNode is the start of the 'terminating condition expression'
    //while the TEST node is the end of the 'terminating condition expression'
    loopControl.condTopNode = movNode->getSingleChild();
    loopControl.condEndNode = testNode;

    //process the step commaExpr if it exists, otherwise set to 1
    std::string stepExpression = "1";
    if (ctx->children.size() == 6) {
        stepExpression = std::any_cast<std::string>(ctx->children[5]->accept(this));
    } 

    //create a ArithOpNode for the step expression
    std::string tempLoopVar = getNewTempVariableName();
    std::shared_ptr<ArithOpNode> arithOpNode = std::make_shared<ArithOpNode>("ADD", tempLoopVar, variableName, stepExpression);
    previousParentNode.lock()->addChild(arithOpNode);
    previousParentNode = arithOpNode;
    //then MOVE the tempLoopVar back to the loop variable
    std::shared_ptr<MovNode> stepMovNode = std::make_shared<MovNode>(variableName, tempLoopVar);
    previousParentNode.lock()->addChild(stepMovNode);
    previousParentNode = stepMovNode;

    //the stepTopNode is the child of the condEndNode
    //and the stepEndNode is the MovNode
    loopControl.stepTopNode = testNode->getSingleChild();

    //return a struct in order to construct a LoopCondNode in BlockDoConstruct
    return loopControl;
}

std::any Fortran90ParserIRTreeVisitor::visitDataStmt(Fortran90Parser::DataStmtContext *ctx) {
    //dataStmt
    //of the form: DATA <dataStmtSet> (COMMA <dataStmtSet>)*
    
    for (size_t i = 1; i < ctx->children.size(); i+=2) {   //<dataStmtSet> is every other child starting at index 1
        ctx->children[i]->accept(this);
    }
    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitDataStmtSet(Fortran90Parser::DataStmtSetContext *ctx) {
    //dataStmtSet
    //of the form: <dse1> <dse2>
    //<dse1> will be an array variable name etc.
    //<dse2> will be the data values

    //will create a DataArrayNode from both of these information
    std::string arrayName = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::vector<std::string> dataValues = std::any_cast<std::vector<std::string>>(ctx->children[1]->accept(this));

    std::shared_ptr<DataArrayNode> dataArrayNode = std::make_shared<DataArrayNode>(arrayName, dataValues);
    previousParentNode.lock()->addChild(dataArrayNode);
    previousParentNode = dataArrayNode;

    return nullptr;
}

std::any Fortran90ParserIRTreeVisitor::visitDse1(Fortran90Parser::Dse1Context *ctx) {
    //dse1
    //of the form: <dataStmtObject> (COMMA <dataStmtObject>)* DIV
    //<dataStmtObject> will be a variable name (e.g. of an array)
    ///NOTE: I will just assume the format of initialising one array per DATA statement
    //so of the form: <dataStmtObject> DIV
    
    //just return the array name
    return std::any_cast<std::string>(ctx->children[0]->accept(this));
    
}

std::any Fortran90ParserIRTreeVisitor::visitDse2(Fortran90Parser::Dse2Context *ctx) {
    //dse2
    //of the form: <dataStmtValue> (COMMA <dataStmtValue>)* DIV
    //<dataStmtValue> will be a CONSTANT

    //just return a vector of constants that occur at every other index starting at index 0
    std::vector<std::string> dataStmtValues;
    for (size_t i = 0; i < ctx->children.size(); i+=2) {
        dataStmtValues.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this)));
    }
    return dataStmtValues;
    
}

std::any Fortran90ParserIRTreeVisitor::visitConstant(Fortran90Parser::ConstantContext *ctx) {
    //constant
    //of the form: (PLUS | MINUS)? unsignedArithmeticConstant
    //just return the constant if its a negative number

    if (ctx->children[0]->getText() == "+") {    //positive number just return as normal
        return std::any_cast<std::string>(ctx->children[1]->accept(this));
    }
    else if (ctx->children[0]->getText() == "-") {    //negative number just return as normal
        return "-" + std::any_cast<std::string>(ctx->children[1]->accept(this));   //still works with stoi
    }
    else{
        throw std::runtime_error("ERROR: unknown constant: " + ctx->getText());
    }
}

std::any Fortran90ParserIRTreeVisitor::visitSFExprListRef(Fortran90Parser::SFExprListRefContext *ctx) {
    //this is accessing indices for storing to multi-dimensional arrays
    //of the form: ( <expression> <commaSectionSubscript>* )
    //<expression> will be a subscript value
    //<commaSectionSubscript> will be a COMMA followed by a subscript value
        //so need to concatenate all the subscript values
    
    std::vector<std::string> subscriptValues;
    for (size_t i = 1; i < ctx->children.size()-1; i++) {     //between the LPAREN and the RPAREN brackets
        subscriptValues.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this)));
    }
    return subscriptValues;
    
}

std::any Fortran90ParserIRTreeVisitor::visitCommaSectionSubscript(Fortran90Parser::CommaSectionSubscriptContext *ctx) {
    //of the form: COMMA <sectionSubscript>
    //just return the sectionSubscript
    return std::any_cast<std::string>(ctx->children[1]->accept(this));    
}

std::any Fortran90ParserIRTreeVisitor::visitNameDataRef(Fortran90Parser::NameDataRefContext *ctx) {
    //NameDataRef for accessing/reading/loading from multi-dimensional arrays
    //of the form: <name> <sectionSubscriptRef>*
    //e.g. <name> will be the array name, and <sectionSubscriptRef> will be the section subscript values
    
    std::string arrayName = std::any_cast<std::string>(ctx->children[0]->accept(this));
    
    //just assume one <sectionSubscriptRef> for now
    // e.g. accessing y(1, 2, 3), the <sectionSubscriptRef> should be (1, 2, 3)
    std::vector<std::string> sectionSubscriptValues = std::any_cast<std::vector<std::string>>(ctx->children[1]->accept(this));

    //create a LoadEltNode which stores the result of the array access into a temp variable
    std::string newTempDest = getNewTempVariableName();
    std::shared_ptr<LoadEltNode> loadEltNode = std::make_shared<LoadEltNode>(newTempDest, arrayName, sectionSubscriptValues);
    previousParentNode.lock()->addChild(loadEltNode);
    previousParentNode = loadEltNode;

    return newTempDest;
}

std::any Fortran90ParserIRTreeVisitor::visitSectionSubscriptRef(Fortran90Parser::SectionSubscriptRefContext *ctx) {
    //SectionSubscriptRef for accessing/reading/loading from multi-dimensional arrays
    //of the form: LPAREN sectionSubscriptList RPAREN
    //<sectionSubscriptList> will be a list of section subscript values
    //just return the section subscript values
    if (dynamic_cast<Fortran90Parser::SectionSubscriptListContext*>(ctx->children[1]) != nullptr){
        return std::any_cast<std::vector<std::string>>(ctx->children[1]->accept(this));
    }else{

        //just a single subscript value
        return std::vector<std::string> {std::any_cast<std::string>(ctx->children[1]->accept(this))};
    }
    
}

std::any Fortran90ParserIRTreeVisitor::visitSectionSubscriptList(Fortran90Parser::SectionSubscriptListContext *ctx) {
    //SectionSubscriptList
    //of the form: <sectionSubscript> (COMMA <sectionSubscript>)*
    //<sectionSubscript> will be a subscript value (e.g. could be expression or constant)
    //just return a vector of the section subscript values
    
    std::vector<std::string> sectionSubscriptValues;
    for (size_t i = 0; i < ctx->children.size(); i+=2) {    //every other child starting at index 0
        sectionSubscriptValues.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this)));
    }
    return sectionSubscriptValues;
    
}


//////////////////////HELPER FUNCTIONS//////////////////////

std::string Fortran90ParserIRTreeVisitor::getNewTempVariableName()
{
    return "_t" + std::to_string(tempVariableCount++);
}

std::string Fortran90ParserIRTreeVisitor::getStringIndex(std::string str)
{

    // strip the quotes
    std::string stripped = str.substr(1, str.size() - 2);

    // store the string in the stringIndexMap
    std::string newIndex = "$str" + std::to_string(stringIndexMap.size() + 1);
    stringIndexMap[newIndex] = stripped;

    // return the index of the string in the stringIndexMap
    return newIndex;
}

std::string Fortran90ParserIRTreeVisitor::getItemToPrint(std::string outputItem)
{
    // check if the outputItem is a string or a variable/number etc.

    if (outputItem.at(0) == '"' && outputItem.back() == '"')
    { //"string"

        std::string newIndex = getStringIndex(outputItem);
        return newIndex;
    }

    // otherwise outputItem is a variable/number etc.
    return outputItem;
}

std::string Fortran90ParserIRTreeVisitor::getRelationalOperator(std::string operation)
{
    // get the relational operator, convert from <, > to LT, GT etc.
    if (operation == "<")
    {
        return "LT";
    }
    else if (operation == "<=")
    {
        return "LE";
    }
    else if (operation == "==")
    {
        return "EQ";
    }
    else if (operation == "/=")
    {
        return "NE";
    }
    else if (operation == ">=")
    {
        return "GE";
    }
    else if (operation == ">")
    {
        return "GT";
    }

    // should never reach here
    return "UNKNOWN";
}

std::string Fortran90ParserIRTreeVisitor::getDatatype(std::string typeSpec, int byteSize) {
    //get the datatype from the typeSpec
    //INTEGER, LOGICAL, REAL, DOUBLEPRECISION, DOUBLE PRECISION
    //unsupported: CHARACTER, COMPLEX
    std::string upperTypeSpec = typeSpec;
    std::transform(typeSpec.begin(), typeSpec.end(), upperTypeSpec.begin(), ::toupper);

   
    if ((byteSize <= 4) && (upperTypeSpec == "INTEGER" || upperTypeSpec == "LOGICAL")) {
        return "INT";               //byteSize of 0,1,2,4 etc. all map to i32 in wasm
    }
    else if ((byteSize <=8) && (upperTypeSpec == "INTEGER" || upperTypeSpec == "LOGICAL")) {
        return "INT64";               //INTEGER(8) map to i64 in wasm
    }
    else if ((byteSize <= 4) && (upperTypeSpec == "REAL")) {
        return "REAL";              //can be represented as f32
    }
    else if ((byteSize <= 8) && (upperTypeSpec == "REAL" || upperTypeSpec == "DOUBLEPRECISION" || upperTypeSpec == "DOUBLE PRECISION")) {
        return "DOUBLE";              //can be represented as f64
    }
    else {
        throw std::runtime_error("ERROR: unknown datatype: " + upperTypeSpec + "(" + std::to_string(byteSize) + ")");
    }

    //CHARACTER is not supported yet
    
}

int Fortran90ParserIRTreeVisitor::getNextTempVariableCount()
{
    return tempVariableCount;
}

int Fortran90ParserIRTreeVisitor::getNextLoopLabelNumber() {
    return loopCount;
}

int Fortran90ParserIRTreeVisitor::getNextIfLabelNumber() {
    return ifCount;
}