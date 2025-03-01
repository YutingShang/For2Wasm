#pragma once
#include "antlr4-runtime.h"
#include "Fortran90ParserBaseVisitor.h"

// add includes for the other nodes in the .cpp file
#include "BaseNode.h"

#include <algorithm>  // for std::transform
#include <cctype>     // for std::tolower
#include <functional> // for std::function
#include <any>

class Fortran90ParserIRTreeVisitor : public Fortran90ParserBaseVisitor
{
public:
    // constructor
    Fortran90ParserIRTreeVisitor(Fortran90Parser &parser, std::shared_ptr<BaseNode> startNode) : parser(parser) , previousParentNode(startNode){};

    virtual std::any visitChildren(antlr4::tree::ParseTree *node) override;

    virtual std::any visitTerminal(antlr4::tree::TerminalNode *node) override;

    virtual std::any visitAssignmentStmt(Fortran90Parser::AssignmentStmtContext *ctx) override;

    virtual std::any visitLevel2Expr(Fortran90Parser::Level2ExprContext *ctx) override;

    virtual std::any visitAddOperand(Fortran90Parser::AddOperandContext *ctx) override;

    virtual std::any visitPrimary(Fortran90Parser::PrimaryContext *ctx) override;

    virtual std::any visitPrintStmt(Fortran90Parser::PrintStmtContext *ctx) override;

    virtual std::any visitOutputItemList1(Fortran90Parser::OutputItemList1Context *ctx) override;

    virtual std::any visitReadStmt(Fortran90Parser::ReadStmtContext *ctx) override;

    virtual std::any visitCommaInputItemList(Fortran90Parser::CommaInputItemListContext *ctx) override;

    virtual std::any visitInputItemList(Fortran90Parser::InputItemListContext *ctx) override;

    virtual std::any visitTypeDeclarationStmt(Fortran90Parser::TypeDeclarationStmtContext *ctx) override;

    virtual std::any visitEntityDeclList(Fortran90Parser::EntityDeclListContext *ctx) override;

    virtual std::any visitEquivOperand(Fortran90Parser::EquivOperandContext *ctx) override;

    virtual std::any visitOrOperand(Fortran90Parser::OrOperandContext *ctx) override;

    virtual std::any visitAndOperand(Fortran90Parser::AndOperandContext *ctx) override;

    virtual std::any visitMultOperand(Fortran90Parser::MultOperandContext *ctx) override;

    virtual std::any visitLevel4Expr(Fortran90Parser::Level4ExprContext *ctx) override;

    virtual std::any visitIfConstruct(Fortran90Parser::IfConstructContext *ctx) override;

    virtual std::any visitIfThenStmt(Fortran90Parser::IfThenStmtContext *ctx) override;

    virtual std::any visitElseIfConstruct(Fortran90Parser::ElseIfConstructContext *ctx) override;

    virtual std::any visitElseIfStmt(Fortran90Parser::ElseIfStmtContext *ctx) override;

    virtual std::any visitElseConstruct(Fortran90Parser::ElseConstructContext *ctx) override;

    virtual std::any visitEndIfStmt(Fortran90Parser::EndIfStmtContext *ctx) override;

    virtual std::any visitBlockDoConstruct(Fortran90Parser::BlockDoConstructContext *ctx) override;

    virtual std::any visitCommaLoopControl(Fortran90Parser::CommaLoopControlContext *ctx) override;

    virtual std::any visitLoopControl(Fortran90Parser::LoopControlContext *ctx) override;

    virtual std::any visitTypeSpec(Fortran90Parser::TypeSpecContext *ctx) override;

    virtual std::any visitKindSelector(Fortran90Parser::KindSelectorContext *ctx) override;

    virtual std::any visitUnsignedArithmeticConstant(Fortran90Parser::UnsignedArithmeticConstantContext *ctx) override;

    virtual std::any visitEntityDecl(Fortran90Parser::EntityDeclContext *ctx) override;

    virtual std::any visitExplicitShapeSpecList(Fortran90Parser::ExplicitShapeSpecListContext *ctx) override;

    virtual std::any visitDataStmt(Fortran90Parser::DataStmtContext *ctx) override;

    virtual std::any visitDataStmtSet(Fortran90Parser::DataStmtSetContext *ctx) override;

    virtual std::any visitDse1(Fortran90Parser::Dse1Context *ctx) override;

    virtual std::any visitDse2(Fortran90Parser::Dse2Context *ctx) override;

    ////for storing/writing into arrays
    virtual std::any visitSFExprListRef(Fortran90Parser::SFExprListRefContext *ctx) override;

    virtual std::any visitCommaSectionSubscript(Fortran90Parser::CommaSectionSubscriptContext *ctx) override;    

    ////for reading from arrays
    virtual std::any visitNameDataRef(Fortran90Parser::NameDataRefContext *ctx) override;

    virtual std::any visitSectionSubscriptRef(Fortran90Parser::SectionSubscriptRefContext *ctx) override;

    virtual std::any visitSectionSubscriptList(Fortran90Parser::SectionSubscriptListContext *ctx) override; 

    virtual std::any visitConstant(Fortran90Parser::ConstantContext *ctx) override;   //for negative numbers in the data segment   
    

    std::unordered_map<std::string, std::string> getStringMap(){
        return stringIndexMap;
    };

    //just returns the tempVariableCount, public interface for CSEOptimizer
    //different to private getNewTempVariableName(), which modifies the tempVariableCount
    int getNextTempVariableCount();

    int getNextLoopLabelNumber();

    int getNextIfLabelNumber();

private:
    // the parser to access the rule names
    Fortran90Parser &parser;

    int tempVariableCount = 0;            // just to keep track of temporary variables used for intermediate results - not mapped to any user defined variables
    std::string getNewTempVariableName(); // fresh temp variable name

    //methods for converting to IR code:
    
    //<index> <string_to_print> where the index is $str1, $str2, etc. and the string to print is the actual string
    // store strings to be printed as '$str1', '$str2', etc:
    // will strip the quotes of the "strings_to_print"
    std::unordered_map<std::string, std::string> stringIndexMap;
    std::string getStringIndex(std::string str);
    std::string getItemToPrint(std::string outputItem);       // check if the outputItem is a string or a variable/number etc.
    std::string getRelationalOperator(std::string operation); // get the relational operator, convert from <, > to LT, GT etc.
    std::string getDatatype(std::string typeSpec, int byteSize = 0);   //might be like INTEGER(8) or REAL(4) etc. default 0 ignore
 
    int ifCount = 0; // for naming the [if..else..endif] block labels, each control structure will have the same number
    int loopCount = 0;  //the next loop label number for LoopCondNode or LoopNode

    std::weak_ptr<BaseNode> previousParentNode;     // store the previous parent node in the flow diagram, so we can assign the current node as a child of the previous parent node

    // for constructing the LoopCondNode - struct needs to store nodes for temporary construction, e.g. after separating node connections, still need node to exist
    struct doLoopStruct {
        std::shared_ptr<BaseNode> initialisationEndNode; // will be a MoveNode
        std::shared_ptr<BaseNode> condTopNode;
        std::shared_ptr<BaseNode> condEndNode;        //will be a > test node
        std::shared_ptr<BaseNode> stepTopNode;
    };

    bool isOutputItemListForPrintStmt = false;  //when visiting printStmt set this to true when we recurse to outputItemList1 - so print stmt added directly after each output item

    bool createIfElseNode = true;    // for if..elseif..else..endif, you create IfElseNode, but if it is if..elseif..endif, you need to create an IfNode
};