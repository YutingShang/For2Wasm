#include "IrNestedLoopVisitor.h"

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
#include "DataArrayNode.h"
#include "StoreEltNode.h"
#include "LoadEltNode.h"
#include <iostream>

IrNestedLoopVisitor::IrNestedLoopVisitor() {
}

std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> IrNestedLoopVisitor::getPerfectlyNestedLoops() {
    return perfectlyNestedLoops;
}

std::string IrNestedLoopVisitor::acceptAllChildren(const std::shared_ptr<BaseNode>& node) {
    parentIsLoopCondNode = false;   //for all nodes, we are not a LoopCondNode parent to its children
    for (const auto& child : node->getChildren()) {
        child->accept(*this);
    }
    return "";
}

std::string IrNestedLoopVisitor::visitLoopCondNode(const std::shared_ptr<LoopCondNode>& node) {
    if (numberOfLoopsNested == 0) {   //then this is the outer loop
        numberOfLoopsNested++;
        //add the loopCondNode to the perfectlyNestedLoops map
        perfectlyNestedLoops[node] = std::vector<std::shared_ptr<LoopCondNode>>();
        currentOuterLoopCondNode = node;
     
    }else{
        numberOfLoopsNested++;    //always increment the number of loops nested
        //if we are not the outer loop, then we need to check if we are a perfectly nested loop
        ///1. the parent of the current LoopCondNode was a LoopCondNode (not some other node type)
        ///2. the ENDLOOP node of the current LoopCondNode has an ENDBODY node as its immediate child (i.e. end of this inner loop also breaks the outer loop body)
        if (parentIsLoopCondNode && node->getEndLoopNode()->getSingleChild()->getText() == "ENDBODY") {
            //if we are a perfectly nested loop, so we add the loopCondNode to the perfectlyNestedLoops map
            perfectlyNestedLoops[currentOuterLoopCondNode].push_back(node);
        }else{
            //not a perfectly nested loop, just return? without accepting children (body or the endloop)
            numberOfLoopsNested--;   //going out of this loop, so decrement the number of loops nested
            return "";
        }
    }
    // numberOfLoopsNested++;    //always increment the number of loops nested
    parentIsLoopCondNode = true;

    //visit the body of the loop
    node->getBodyNode()->accept(*this);
    node->getEndLoopNode()->accept(*this);

    return "";


}

std::string IrNestedLoopVisitor::visitArithOpNode(const std::shared_ptr<ArithOpNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitLogicNotNode(const std::shared_ptr<LogicNotNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitRelOpNode(const std::shared_ptr<RelOpNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitMovNode(const std::shared_ptr<MovNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitEndBlockNode(const std::shared_ptr<EndBlockNode>& node) {
    if (node->getText() == "ENDLOOP") {
        numberOfLoopsNested--;
    }
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitExitNode(const std::shared_ptr<ExitNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitLoopNode(const std::shared_ptr<LoopNode>& node) {
    numberOfLoopsNested++;
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitIfNode(const std::shared_ptr<IfNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitIfElseNode(const std::shared_ptr<IfElseNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitDeclareNode(const std::shared_ptr<DeclareNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitPrintNode(const std::shared_ptr<PrintNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitReadNode(const std::shared_ptr<ReadNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitTestNode(const std::shared_ptr<TestNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitEntryNode(const std::shared_ptr<EntryNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitDataArrayNode(const std::shared_ptr<DataArrayNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitStoreEltNode(const std::shared_ptr<StoreEltNode>& node) {
    return acceptAllChildren(node);
}

std::string IrNestedLoopVisitor::visitLoadEltNode(const std::shared_ptr<LoadEltNode>& node) {
    return acceptAllChildren(node);
}