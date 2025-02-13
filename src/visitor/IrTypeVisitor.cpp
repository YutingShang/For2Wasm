#include "IrTypeVisitor.h"

#include "IRSemantics.h"
#include "ArithOpNode.h"
#include "LogicBinOpNode.h"
#include "LogicNotNode.h"
#include "RelOpNode.h"
#include "MovNode.h"
#include "EndBlockNode.h"
#include "ExitNode.h"
#include "LoopNode.h"
#include "LoopCondNode.h"
#include "IfNode.h"
#include "IfElseNode.h"
#include "TestNode.h"
#include "EntryNode.h"
#include "DeclareNode.h"
#include "PrintNode.h"
#include "ReadNode.h"
#include "BaseNode.h"
#include "DeclareArrayNode.h"
#include <iostream>

IrTypeVisitor::IrTypeVisitor() {
    //constructor
}

std::string IrTypeVisitor::acceptAllChildren(const std::shared_ptr<BaseNode>& node) {
    for (const auto& child : node->getChildren()) {
        child->accept(*this);
    }
    return "";
}

std::unordered_map<std::string, std::string> IrTypeVisitor::getVariableIRDatatypeMap() {
    return variableDatatypeMap;
}

std::string IrTypeVisitor::visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) {
    std::string dest = node->getDest();
    if (IRSemantics::isInternalTempVar(dest)) {
        //if dest is a temp, we need to determine the datatype of the temp variable

        //take the largest data type of the operands
        std::string src1 = node->getSrc1();
        std::string src2 = node->getSrc2();

        std::string tempDestDatatype = IRSemantics::findLargestDatatype(getIRNumberDatatype(src1), getIRNumberDatatype(src2));
        variableDatatypeMap[dest] = tempDestDatatype;

    }
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) {
    std::string dest = node->getDest();
    std::string src1 = node->getSrc1();
    std::string src2 = node->getSrc2();

    if (IRSemantics::isInternalTempVar(dest)) {
        std::string tempDestDatatype = IRSemantics::findLargestDatatype(getIRNumberDatatype(src1), getIRNumberDatatype(src2));
        variableDatatypeMap[dest] = tempDestDatatype;
    }
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) {
    std::string dest = node->getDest();
    std::string src = node->getSrc();

    if (IRSemantics::isInternalTempVar(dest)) {
        std::string tempDestDatatype = getIRNumberDatatype(src);    //just take the datatype of the src
        variableDatatypeMap[dest] = tempDestDatatype;    //add to the map for the temp variable
    }
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitRelOpNode(const std::shared_ptr<RelOpNode>& node) {
    std::string dest = node->getDest();
    std::string src1 = node->getSrc1();
    std::string src2 = node->getSrc2();

    if (IRSemantics::isInternalTempVar(dest)) {
        std::string tempDestDatatype = IRSemantics::findLargestDatatype(getIRNumberDatatype(src1), getIRNumberDatatype(src2));
        variableDatatypeMap[dest] = tempDestDatatype;
    }
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitMovNode(const std::shared_ptr<MovNode>& node) {
    std::string dest = node->getDest();
    std::string src = node->getSrc();

    if (IRSemantics::isInternalTempVar(dest)) {
        std::string tempDestDatatype = getIRNumberDatatype(src);
        variableDatatypeMap[dest] = tempDestDatatype;
    }
    return acceptAllChildren(node);
}


std::string IrTypeVisitor::visitEndBlockNode(const std::shared_ptr<EndBlockNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitExitNode(const std::shared_ptr<ExitNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitLoopNode(const std::shared_ptr<LoopNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitIfNode(const std::shared_ptr<IfNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitIfElseNode(const std::shared_ptr<IfElseNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitDeclareNode(const std::shared_ptr<DeclareNode>& node) {
    std::string var = node->getVar();
    std::string datatype = node->getDatatype();   //declare nodes store the datatype of the variable
    variableDatatypeMap[var] = datatype;
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitPrintNode(const std::shared_ptr<PrintNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitReadNode(const std::shared_ptr<ReadNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitTestNode(const std::shared_ptr<TestNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitEntryNode(const std::shared_ptr<EntryNode>& node) {
    return acceptAllChildren(node);
}

std::string IrTypeVisitor::visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) {
    return acceptAllChildren(node);
}


/////////////////////////HELPER FUNCTIONS/////////////////////////


std::string IrTypeVisitor::getIRNumberDatatype(const std::string& item) {
    if (IRSemantics::isVariable(item)) { 
        return variableDatatypeMap[item];    //find the datatype in the map
    }
    else if (IRSemantics::isPosInt32(item)) {
        return "INT";
    }
    else if (IRSemantics::isPosInt64(item)) {
        return "INT64";
    }
    else if (IRSemantics::isPosFloat32(item)) {
        return "REAL";
    }
    else if (IRSemantics::isPosFloat64(item)) {
        return "DOUBLE";
    }
    else{
        throw std::runtime_error("Invalid item in getIRNumberDatatype: "+item);
    }
}


bool IrTypeVisitor::isIRIntDatatype(const std::string& type) {
    return type == "INT" || type == "INT64";
}

bool IrTypeVisitor::isIRFloatDatatype(const std::string& type) {
    return type == "REAL" || type == "DOUBLE";
}

