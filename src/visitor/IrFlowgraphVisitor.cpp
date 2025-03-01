#include "IrFlowgraphVisitor.h"

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
#include "SimpleNode.h"
#include "DeclareArrayNode.h"
#include "DataArrayNode.h"
#include "StoreEltNode.h"
#include "LoadEltNode.h"

#include <iostream>

IrFlowgraphVisitor::IrFlowgraphVisitor(std::shared_ptr<BasicBlock> startBasicBlock)
{

    currentBasicBlock = startBasicBlock;
}

std::string IrFlowgraphVisitor::visitSimpleNode(const std::shared_ptr<SimpleNode> &node)
{
    //for simple nodes with 1 child, just add the instruction to the current basic block, and process the child if it exists
    currentBasicBlock->add_instruction(node);

    if (node->getChildren().size() == 1)
    {
        node->getChildren()[0]->accept(*this);
    }

    return ""; // like returning nullptr - not used
}

std::string IrFlowgraphVisitor::visitEntryNode(const std::shared_ptr<EntryNode> &node)
{

    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitArithOpNode(const std::shared_ptr<ArithOpNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLogicBinOpNode(const std::shared_ptr<LogicBinOpNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLogicNotNode(const std::shared_ptr<LogicNotNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitRelOpNode(const std::shared_ptr<RelOpNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitMovNode(const std::shared_ptr<MovNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitEndBlockNode(const std::shared_ptr<EndBlockNode> &node)
{
    if (node->getText() == "ENDLOOP"){
        // pop the loop stack when we finish processing the loop
        exitStack.pop();
    }

    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitDeclareNode(const std::shared_ptr<DeclareNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitDeclareArrayNode(const std::shared_ptr<DeclareArrayNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitDataArrayNode(const std::shared_ptr<DataArrayNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitStoreEltNode(const std::shared_ptr<StoreEltNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLoadEltNode(const std::shared_ptr<LoadEltNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitPrintNode(const std::shared_ptr<PrintNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitReadNode(const std::shared_ptr<ReadNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitTestNode(const std::shared_ptr<TestNode> &node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLoopNode(const std::shared_ptr<LoopNode> &node)
{
    // add the LOOP instruction to end of the current basic block
    currentBasicBlock->add_instruction(node);

    // create a new basic block for the loop body, setting it as current basic block, so the next instruction will be added to the loop body
    startNewBasicBlockSuccessor();

    // create a new basic block for the loop endloop (for the endloop instruction to jump to)
    std::shared_ptr<BasicBlock> loopEndloop = std::make_shared<BasicBlock>();
    exitStack.push(loopEndloop);

    // save the current loop body basic block
    std::shared_ptr<BasicBlock> loopBody = currentBasicBlock;

    // process the body of the loop - should contain an EXIT instruction
    node->getBodyNode()->accept(*this);

    // last instruction of the loop body should connect back to the loop body basic block
    currentBasicBlock->add_successor(loopBody);

    // process the end of the loop, i.e. ENDLOOP or the endloopLabel of the loop
    currentBasicBlock = loopEndloop;
    node->getEndLoopNode()->accept(*this);

    return "";
}

std::string IrFlowgraphVisitor::visitLoopCondNode(const std::shared_ptr<LoopCondNode> &node)
{

    //add the LOOP instruction to the current basic block, add the initialisation code also the the same basic block
    currentBasicBlock->add_instruction(node);
    node->getInitNode()->accept(*this);
    //create a new basic block for the termination condition of the loop
    startNewBasicBlockSuccessor();
    std::shared_ptr<BasicBlock> terminationConditionBasicBlock = currentBasicBlock;     //save the basic block for later
    node->getCondNode()->accept(*this);

    //create a new basic block for the loop endloop (POTENTIALLY exit node within the LoopCondNode)
    std::shared_ptr<BasicBlock> loopEndloop = std::make_shared<BasicBlock>();
    exitStack.push(loopEndloop);

    //create a new basic block for the loop body+step code, process the two children
    startNewBasicBlockSuccessor();
    node->getBodyNode()->accept(*this);
    node->getStepNode()->accept(*this);

    //connect the loop body+step code to the termination condition basic block
    currentBasicBlock->add_successor(terminationConditionBasicBlock);

    //set the current basic block to the termination condition basic block
    //and create a new basic block for the ENDLOOP instruction
    // currentBasicBlock = terminationConditionBasicBlock;
    // startNewBasicBlockSuccessor();
    terminationConditionBasicBlock->add_successor(loopEndloop);
    currentBasicBlock = loopEndloop;
    node->getEndLoopNode()->accept(*this);

    return "";
}

std::string IrFlowgraphVisitor::visitExitNode(const std::shared_ptr<ExitNode> &node)
{
    // set the successor of the current basic block to the endloop basic block
    std::shared_ptr<BasicBlock> loopEndloop = exitStack.top();
    currentBasicBlock->setContainsExitNode(true);
    currentBasicBlock->add_successor(loopEndloop, false);  //FALSE - do not check if it is an exit node, always connect the exit node to the endloop basic block

    // pop the loop stack when we finish processing the loop (could be another exit node within the loop)

    // but continue adding to the current basic block
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitIfNode(const std::shared_ptr<IfNode> &node)
{
    // add the IF instruction to the current basic block
    currentBasicBlock->add_instruction(node);

    // process the condition until TEST- keep adding this to the current basic block with the IF instruction
    node->getChildren()[0]->accept(*this);

    // create a new basic block for the ENDIF instruction to be filled in later
    // this is a direct successor of the TEST block, since there is no ELSE block
    std::shared_ptr<BasicBlock> endIfBasicBlock = std::make_shared<BasicBlock>();
    currentBasicBlock->add_successor(endIfBasicBlock);

    // then create a new basic block for the THEN control flow, which sets a new current basic block
    startNewBasicBlockSuccessor();

    // process the THEN control flow in the new current basic block
    node->getChildren()[1]->accept(*this);

    // last basic block of the "then" branch, will contain the ENDTHEN instruction
    // connect it to the ENDIF basic block
    currentBasicBlock->add_successor(endIfBasicBlock);

    // now set the current basic block to the ENDIF basic block and process from there
    currentBasicBlock = endIfBasicBlock;
    node->getChildren()[2]->accept(*this);

    return "";
}

std::string IrFlowgraphVisitor::visitIfElseNode(const std::shared_ptr<IfElseNode> &node)
{
    // add the IF instruction to the current basic block
    currentBasicBlock->add_instruction(node);

    // create a new basic block for the ENDIF instruction to be filled in later
    std::shared_ptr<BasicBlock> endIfBasicBlock = std::make_shared<BasicBlock>();

    // process the condition until TEST- keep adding this to the current basic block
    node->getChildren()[0]->accept(*this);

    // then create a new basic block for the THEN control flow
    std::shared_ptr<BasicBlock> savedCurrentBasicBlock = currentBasicBlock;
    startNewBasicBlockSuccessor();

    // process the THEN control flow
    node->getChildren()[1]->accept(*this);

    // the last basic block of the "then" branch, will contain the ENDTHEN instruction
    // connect it to the ENDIF basic block
    currentBasicBlock->add_successor(endIfBasicBlock);

    // process the ELSE control flow, first set the current basic block back to the saved basic block with the TEST instruction
    currentBasicBlock = savedCurrentBasicBlock;
    startNewBasicBlockSuccessor();
    node->getChildren()[2]->accept(*this);

    // the last basic block of the "else" branch, will contain the ENDELSE instruction
    // connect it to the ENDIF basic block
    currentBasicBlock->add_successor(endIfBasicBlock);

    // now set the current basic block to the ENDIF basic block and process from there
    currentBasicBlock = endIfBasicBlock;
    node->getChildren()[3]->accept(*this);

    return "";
}

void IrFlowgraphVisitor::startNewBasicBlockSuccessor()
{

    // create a new basic block and add it as a successor to the current basic block
    std::shared_ptr<BasicBlock> newBasicBlock = std::make_shared<BasicBlock>();
    currentBasicBlock->add_successor(newBasicBlock);
    currentBasicBlock = newBasicBlock;
}