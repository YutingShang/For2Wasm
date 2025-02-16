#pragma once

#include "IrBaseVisitor.h"
#include <map>
#include <unordered_set>

//visitor will determine the types of the variables in the IR tree
//and add them to the variableDatatypeMap

class IrTypeVisitor : public IrBaseVisitor {
    public:
        IrTypeVisitor();
        ~IrTypeVisitor() override = default;

        std::string visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) override;

        std::string visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) override;

        std::string visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) override;

        std::string visitRelOpNode(const std::shared_ptr<RelOpNode>& node) override;

        std::string visitMovNode(const std::shared_ptr<MovNode>& node) override;

        std::string visitEndBlockNode(const std::shared_ptr<EndBlockNode>& node) override;

        std::string visitExitNode(const std::shared_ptr<ExitNode>& node) override;

        std::string visitLoopNode(const std::shared_ptr<LoopNode>& node) override;

        std::string visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) override;

        std::string visitIfNode(const std::shared_ptr<IfNode>& node) override;

        std::string visitIfElseNode(const std::shared_ptr<IfElseNode>& node) override;
        
        std::string visitDeclareNode(const std::shared_ptr<DeclareNode>& node) override;

        std::string visitPrintNode(const std::shared_ptr<PrintNode>& node) override;

        std::string visitReadNode(const std::shared_ptr<ReadNode>& node) override;

        std::string visitTestNode(const std::shared_ptr<TestNode>& node) override;

        std::string visitEntryNode(const std::shared_ptr<EntryNode>& node) override;

        std::string visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) override;

        std::string visitDataArrayNode(const std::shared_ptr<DataArrayNode>& node) override;

        std::string visitStoreEltNode(const std::shared_ptr<StoreEltNode>& node) override;

        std::string visitLoadEltNode(const std::shared_ptr<LoadEltNode>& node) override;

        std::unordered_map<std::string, std::string> getVariableIRDatatypeMap();

        std::unordered_set<std::string> getArrayVariables();    //not sure if used

private:

    //accepts all children of a node and returns an empty string
    std::string acceptAllChildren(const std::shared_ptr<BaseNode>& node);

    //maps all variables (including internal temp variables) to their IR datatype (INT, INT64, REAL, DOUBLE)
    //initially add PROGRAM VARIABLES to the map when visiting DECLARE statements
    //then add INTERNAL TEMP VARIABLES to the map when visiting other statements and creating those temporary variables
    std::unordered_map<std::string, std::string> variableDatatypeMap;

    //a set of all array variables in the program
    std::unordered_set<std::string> arrayVariables;

    //get the IR datatype of a variable or constant (e.g. input x or _t0 or 3.14, returns INT or REAL etc.)
    std::string getIRNumberDatatype(const std::string& item);

    //true if datatype is INT or INT64
    bool isIRIntDatatype(const std::string& type);

    //true if datatype is REAL or DOUBLE
    bool isIRFloatDatatype(const std::string& type);
};