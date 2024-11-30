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
    std::string text = node->getText();

    // Convert text to uppercase 
    std::string textUPPER = text;
    std::transform(text.begin(), text.end(), textUPPER.begin(), ::toupper);   //stores upper case of text in new string
   
    if (textUPPER == "EXIT"){
        std::cout<< "\t"<<textUPPER <<std::endl;        //return the 'EXIT' IR instruction, if we see 'exit' or 'EXIT' in the AST
    }
    return text;
}

std::any Fortran90ParserTranslatorVisitor::visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx)
{
    // assignmentStmt will be in the form <NAME> <ASSIGN> <expression>
    assert(ctx->children.size() == 3);

    // first visit children - recurse on the expression
    std::string variableName = ctx->children[0]->getText();
    std::string expression = std::any_cast<std::string>(ctx->children[2]->accept(this));

    // then process the current node
    std::cout << "\tMOV " << variableName << " " << expression << std::endl;

    return nullptr;
    /// QUESTION: what to return for assignmentStmt? - maybe an instruction block label?
}

std::any Fortran90ParserTranslatorVisitor::visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx)
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
        std::cout << "\tSUB " << tempResultVar << " 0 " << addOperand1 << std::endl;
    }
    else
    { // does not start with a `-`, process the first ADD or SUB

        std::string otherTempResultVar = getNewTempVariableName();
        lastTempVar = otherTempResultVar;
        std::string operation = ctx->children[1]->getText();
        std::string operationSymbol = (operation == "+") ? "ADD" : "SUB";
        std::string addOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
        std::string addOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
        std::cout <<"\t"<< operationSymbol << " " << otherTempResultVar << " " << addOperand1 << " " << addOperand2 << std::endl;
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
        std::cout <<"\t"<<operationSymbol << " " << newTempVar << " " << lastTempVar << " " << nextAddOperand << std::endl;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar; // return the last temp variable the result of this level2Expr is stored in
}

std::any Fortran90ParserTranslatorVisitor::visitAddOperand(Fortran90Parser::AddOperandContext *ctx)
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
        std::cout << "\t"<<operationSymbol << " " << tempResultVar << " " << multOperand1 << " " << multOperand2 << std::endl;

        // repeat for the next multOperand

        size_t index = 4;
        while (index < ctx->children.size())
        {
            std::string nextMultOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
            std::string newTempVar = getNewTempVariableName();
            std::string operation = ctx->children[index - 1]->getText();
            std::string operationSymbol = (operation == "*") ? "MUL" : "DIV";
            std::cout << "\t"<<operationSymbol << " " << newTempVar << " " << lastTempVar << " " << nextMultOperand << std::endl;

            lastTempVar = newTempVar;
            index += 2;
        }

        return lastTempVar;
    }
}

std::any Fortran90ParserTranslatorVisitor::visitPrimary(Fortran90Parser::PrimaryContext *ctx)
{
    // primary will be in the form:
    // <LPAREN> <expression> <RPAREN>

    // so we just want to return the result of visiting the expression

    return ctx->children[1]->accept(this);
}

std::any Fortran90ParserTranslatorVisitor::visitPrintStmt(Fortran90Parser::PrintStmtContext *ctx)
{
    // get outputItemList which is the children at the 3rd index
    // then see if it has multiple children or just one
    // also see if you are printing out variable or a string

    // if its a terminal, print the terminal, otherwise recurse to outputItemList1

    if (ctx->children[3]->children.size() == 0)
    { // no children, so must be terminal to print directly - no outputItemList1
        std::string outputItem = ctx->children[3]->getText();

        // check if the outputItem is a string or a variable/number etc and print accordingly
        std::cout << "\tCALL PRINT " << getItemToPrint(outputItem) << std::endl;
    }
    else
    { // explore the outputItemList1 node

        // get the list of stuff to print
        std::vector<std::string> outputList = std::any_cast<std::vector<std::string>>(ctx->children[3]->accept(this));

        // print out each item in the list
        for (std::string item : outputList)
        {
            std::cout << "\tCALL PRINT " << item << std::endl;
        }
    }

    return nullptr;
    /// QUESTION: what to return for printStmt? nullptr, like a 'void' instruction?
}

std::any Fortran90ParserTranslatorVisitor::visitOutputItemList1(Fortran90Parser::OutputItemList1Context *ctx)
{

    // outputItemList1 could have more children
    // either outputItemList1 = <expression> , <expression>
    // or outputItemList1 = <outputItemList1> , <expression>  (RECURSIVE)

    // this visit method will return a list of stuff to print out
    // either the stringIndex or the variable name etc.
    // actual printing will be done in the printStmt visit method

    std::vector<std::string> outputList;

    if (ctx->children[0]->children.size() == 0)
    { // no children, so must be terminal to print directly - no outputItemList1

        std::string outputItem = ctx->children[0]->getText();
        // getItemToPrint checks if string or variable/number etc.
        outputList.push_back(getItemToPrint(outputItem));
    }
    else
    { // explore the outputItemList1 node
        outputList = std::any_cast<std::vector<std::string>>(ctx->children[0]->accept(this));
    }

    // right child (at index 2 after the comma) should always be an expression to print
    std::string rightChildExpr = ctx->children[2]->getText();
    outputList.push_back(getItemToPrint(rightChildExpr));

    return outputList;
}

std::any Fortran90ParserTranslatorVisitor::visitReadStmt(Fortran90Parser::ReadStmtContext *ctx)
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
        std::cout << "\tCALL READ " << inputVar << std::endl;
    }
    else
    {
        // get the child at index 2, which is either commaInputItemList or inputItemList
        std::vector<std::string> inputItemList = std::any_cast<std::vector<std::string>>(ctx->children[2]->accept(this));

        for (std::string item : inputItemList)
        {
            std::cout << "\tCALL READ " << item << std::endl;
        }
    }

    return nullptr;

    // we want to return the list of variables to read into
}

std::any Fortran90ParserTranslatorVisitor::visitCommaInputItemList(Fortran90Parser::CommaInputItemListContext *ctx)
{
    // just ignore the comma and return the inputItemList

    // if there is only 1 variable, then no InputItemList, just a single variable name
    if (ctx->children[1]->children.size() == 0)
    {
        return std::vector<std::string>{ctx->children[1]->getText()};
    }

    return std::any_cast<std::vector<std::string>>(ctx->children[1]->accept(this));
}

std::any Fortran90ParserTranslatorVisitor::visitInputItemList(Fortran90Parser::InputItemListContext *ctx)
{
    // return the list of input variables

    std::vector<std::string> inputList;

    for (size_t i = 0; i < ctx->children.size(); i += 2)
    {
        inputList.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this)));
    }

    return inputList;
}

std::any Fortran90ParserTranslatorVisitor::visitTypeDeclarationStmt(Fortran90Parser::TypeDeclarationStmtContext *ctx)
{
    // typeDeclarationStmt will be in the form:
    // <TYPE> <entityDeclList>
    // or <TYPE> <attrSpec>? :: <entityDeclList>

    /// NOTE:!! currently just assuming TYPE is always INTEGER - generic 'DECLARE' command

    // entityDeclList will be a list of entityDecl separated by commas
    // or just a single variable to declare and no entityDeclList

    size_t numChildren = ctx->children.size();

    // entityDecList will always be at the last index in the children vector
    int entityDeclListIndex = numChildren - 1;

    // first check if entityDeclList is actually just a single variable
    // in that case, do not recurse
    if (ctx->children[entityDeclListIndex]->children.size() == 0)
    {

        std::string variable = ctx->children[entityDeclListIndex]->getText();
        std::cout << "\tDECLARE " << variable << std::endl;
    }
    else
    {
        // recurse on the entityDeclList
        std::vector<std::string> variables = std::any_cast<std::vector<std::string>>(ctx->children[entityDeclListIndex]->accept(this));
        for (std::string variable : variables)
        {
            std::cout << "\tDECLARE " << variable << std::endl;
        }
    }

    return nullptr;
}

std::any Fortran90ParserTranslatorVisitor::visitEntityDeclList(Fortran90Parser::EntityDeclListContext *ctx)
{
    // entityDeclList will be a list of <entityDecl> separated by commas
    /// NOTE: this is assuming <entityDecl> is just <objectName> - no ASSIGN (initialization of variables during declaration) etc.
    /// TODO: handle more complex entityDecl (including initialisation etc. if WASM allows it)

    std::vector<std::string> variables;

    for (size_t i = 0; i < ctx->children.size(); i += 2)
    {
        // for every <entityDecl> child of entityDeclList, assume it is only <objectName> (for now)
        assert(ctx->children[i]->children.size() == 0); // should not have any children

        variables.push_back(std::any_cast<std::string>(ctx->children[i]->accept(this))); // this will reach a terminal node and return the variable name
    }

    return variables;
}

std::any Fortran90ParserTranslatorVisitor::visitEquivOperand(Fortran90Parser::EquivOperandContext *ctx)
{
    // of the form <orOperand> (.OR. <orOperand>)*
    // will return a temp variable containing the result of the OR operation

    assert(ctx->children.size() >= 3); // should have at least two orOperands

    // calculate first pair of orOperands
    std::string orOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string orOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
    std::string tempResultVar = getNewTempVariableName();
    std::string lastTempVar = tempResultVar;
    std::cout << "\tOR" << " " << tempResultVar << " " << orOperand1 << " " << orOperand2 << std::endl;

    // repeat for the next multOperand
    size_t index = 4;
    while (index < ctx->children.size())
    {
        std::string nextOrOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();
        std::cout << "\tOR" << " " << newTempVar << " " << lastTempVar << " " << nextOrOperand << std::endl;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;
}

std::any Fortran90ParserTranslatorVisitor::visitOrOperand(Fortran90Parser::OrOperandContext *ctx)
{
    // of the form <andOperand> (.AND. <andOperand>)*

    assert(ctx->children.size() >= 3); // should have at least two orOperands

    // calculate first pair of andOperands
    std::string andOperand1 = std::any_cast<std::string>(ctx->children[0]->accept(this));
    std::string andOperand2 = std::any_cast<std::string>(ctx->children[2]->accept(this));
    std::string tempResultVar = getNewTempVariableName();
    std::string lastTempVar = tempResultVar;
    std::cout << "\tAND" << " " << tempResultVar << " " << andOperand1 << " " << andOperand2 << std::endl;

    // repeat for the next andOperand
    size_t index = 4;
    while (index < ctx->children.size())
    {
        std::string nextAndOperand = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();
        std::cout << "\tAND" << " " << newTempVar << " " << lastTempVar << " " << nextAndOperand << std::endl;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;
}

std::any Fortran90ParserTranslatorVisitor::visitAndOperand(Fortran90Parser::AndOperandContext *ctx)
{
    // of the form
    // LNOT <level4Expr>  (should have the NOT, if visit this node, otherwise go directly to level4Expr)

    assert(ctx->children.size() == 2); // should have the NOT and the level4Expr

    // will output
    // NOT <not_expr> <expr>

    std::string expr = std::any_cast<std::string>(ctx->children[1]->accept(this));
    std::string tempVar = getNewTempVariableName();

    std::cout << "\tNOT" << " " << tempVar << " " << expr << std::endl;

    return tempVar;
}

std::any Fortran90ParserTranslatorVisitor::visitLevel4Expr(Fortran90Parser::Level4ExprContext *ctx)
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
    std::cout << "\t"<<operationSymbol << " " << tempResultVar << " " << level3Expr1 << " " << level3Expr2 << std::endl;

    // repeat for the next level3Expr
    size_t index = 4;
    while (index < ctx->children.size())
    {

        std::string nextLevel3Expr = std::any_cast<std::string>(ctx->children[index]->accept(this));
        std::string newTempVar = getNewTempVariableName();

        std::string operation = ctx->children[index - 1]->getText();
        std::string operationSymbol = getRelationalOperator(operation);
        std::cout << "\t"<<operationSymbol << " " << newTempVar << " " << lastTempVar << " " << nextLevel3Expr << std::endl;

        lastTempVar = newTempVar;
        index += 2;
    }

    return lastTempVar;
}

std::any Fortran90ParserTranslatorVisitor::visitIfConstruct(Fortran90Parser::IfConstructContext *ctx)
{
    // <ifThenStmt> <conditionalBody> <elseIfConstruct*> <elseConstruct?> <endIfStmt>

    // process the ifThenStmt

    // std::string condition = std::any_cast<std::string>(ctx->children[0]->accept(this));     //actually need to print this condition after the IF label??
    std::string conditionIndex = "cond" + std::to_string(conditionCount++);
    std::string thenIndex = "then" + std::to_string(thenCount++);
    std::string elseIndex = "else" + std::to_string(elseCount++);

    std::cout << "\tIF " << conditionIndex << " " << thenIndex << " " << elseIndex << std::endl;
    visitIfThenStmtWithLabel(dynamic_cast<Fortran90Parser::IfThenStmtContext *>(ctx->children[0]), conditionIndex, thenIndex);

    //visit the conditionalBody at index 2, if that node is a terminal, just output it (e.g. EXIT)
    ///NOTE: visitTerminal will implement printing the EXIT statement
    //otherwise conditionalBody can contain multiple <executionPartConstruct> it will visitChildren
    //or conditionalBody will contain some other (non-terminal) statement (e.g. print) it will visit
    ctx->children[1]->accept(this);

    return nullptr;
}

// wrapper function with extra parameter to pass in the instruction label
/// TODO: can make this into a generic wrapper function for any visit method!!
void Fortran90ParserTranslatorVisitor::visitIfThenStmtWithLabel(Fortran90Parser::IfThenStmtContext *ctx, const std::string &condLabel, const std::string &thenLabel)
{

    // print the condition label - no std::endl
    std::cout << condLabel << ": ";
    ctx->accept(this);

    std::cout<< thenLabel << ": ";
}

std::any Fortran90ParserTranslatorVisitor::visitIfThenStmt(Fortran90Parser::IfThenStmtContext *ctx)
{
    // IF LPAREN expression RPAREN THEN

    // process the <condition expression> and then return and print the 'cond' label

    // std::string conditionIndex = "cond" + std::to_string(conditionCount++);

    // print the condition label - no std::endl
    //  std::cout<<conditionIndex<<": ";

    // process the expression - would print the block of condition code
    /// NOTE: this would also return the temp var which is the actual condition! But not sure if it is needed anywhere
    ctx->children[2]->accept(this);

    return nullptr;
}

std::any Fortran90ParserTranslatorVisitor::visitElseIfConstruct(Fortran90Parser::ElseIfConstructContext *ctx)
{
    return visitChildren(ctx);
}

std::any Fortran90ParserTranslatorVisitor::visitElseIfStmt(Fortran90Parser::ElseIfStmtContext *ctx)
{
    return visitChildren(ctx);
}

std::any Fortran90ParserTranslatorVisitor::visitElseConstruct(Fortran90Parser::ElseConstructContext *ctx)
{
    return visitChildren(ctx);
}

std::any Fortran90ParserTranslatorVisitor::visitElseStmt(Fortran90Parser::ElseStmtContext *ctx)
{
    return visitChildren(ctx);
}

std::any Fortran90ParserTranslatorVisitor::visitEndIfStmt(Fortran90Parser::EndIfStmtContext *ctx)
{
    return visitChildren(ctx);
}

//////////////////////HELPER FUNCTIONS//////////////////////

std::string Fortran90ParserTranslatorVisitor::getNewTempVariableName()
{
    return "t" + std::to_string(tempVariableCount++);
}

std::string Fortran90ParserTranslatorVisitor::getStringIndex(std::string str)
{

    // strip the quotes
    std::string stripped = str.substr(1, str.size() - 2);

    // store the string in the stringIndexMap
    std::string newIndex = "$str" + std::to_string(stringIndexMap.size() + 1);
    stringIndexMap[newIndex] = stripped;

    // return the index of the string in the stringIndexMap
    return newIndex;
}

std::string Fortran90ParserTranslatorVisitor::getItemToPrint(std::string outputItem)
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

std::string Fortran90ParserTranslatorVisitor::getRelationalOperator(std::string operation)
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