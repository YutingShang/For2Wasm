#include "IrFlowgraphVisitor.h"

#include "ArithOpNode.h"
#include "LogicBinOpNode.h"
#include "LogicNotNode.h"
#include "RelOpNode.h"
#include "MovNode.h"
#include "EndBlockNode.h"
#include "ExitNode.h"
#include "LoopNode.h"
#include "IfNode.h"
#include "IfElseNode.h"
#include "TestNode.h"
#include "EntryNode.h"
#include "DeclareNode.h"
#include "PrintNode.h"
#include "ReadNode.h"
#include "BaseNode.h"
#include "SimpleNode.h"

#include <iostream>

IrFlowgraphVisitor::IrFlowgraphVisitor(BasicBlock *startBasicBlock)
{

    currentBasicBlock = startBasicBlock;
}

std::string IrFlowgraphVisitor::visitSimpleNode(SimpleNode *node)
{
    currentBasicBlock->add_instruction(node);

    if (node->getChildren().size() == 1)
    {
        node->getChildren()[0]->accept(this);
    }

    return ""; // like returning nullptr - not used
}

std::string IrFlowgraphVisitor::visitEntryNode(EntryNode *node)
{

    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitArithOpNode(ArithOpNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLogicBinOpNode(LogicBinOpNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLogicNotNode(LogicNotNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitRelOpNode(RelOpNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitMovNode(MovNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitEndBlockNode(EndBlockNode *node)
{

    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitDeclareNode(DeclareNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitPrintNode(PrintNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitReadNode(ReadNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitTestNode(TestNode *node)
{
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitLoopNode(LoopNode *node)
{
    // add the LOOP instruction to end of the current basic block
    currentBasicBlock->add_instruction(node);

    // create a new basic block for the loop body, setting it as current basic block, so the next instruction will be added to the loop body
    startNewBasicBlockSuccessor();

    // create a new basic block for the loop exit (for the exit instruction to jump to)
    BasicBlock *loopExit = new BasicBlock();
    exitStack.push(loopExit);

    // save the current loop body basic block
    BasicBlock *loopBody = currentBasicBlock;

    // process the body of the loop - should contain an EXIT instruction
    node->getChildren()[0]->accept(this);

    // last instruction of the loop body should connect back to the loop body basic block
    currentBasicBlock->add_successor(loopBody);

    // process the end of the loop, i.e. ENDLOOP or the exitLabel of the loop
    currentBasicBlock = loopExit;
    node->getChildren()[1]->accept(this);

    return "";
}

std::string IrFlowgraphVisitor::visitExitNode(ExitNode *node)
{
    // set the successor of the current basic block to the exit basic block
    BasicBlock *loopExit = exitStack.top();
    currentBasicBlock->add_successor(loopExit);

    // pop the loop stack
    exitStack.pop();

    // but continue adding to the current basic block
    return visitSimpleNode(node);
}

std::string IrFlowgraphVisitor::visitIfNode(IfNode *node)
{
    // add the IF instruction to the current basic block
    currentBasicBlock->add_instruction(node);

    // process the condition until TEST- keep adding this to the current basic block with the IF instruction
    node->getChildren()[0]->accept(this);

    // create a new basic block for the ENDIF instruction to be filled in later
    // this is a direct successor of the TEST block, since there is no ELSE block
    BasicBlock *endIfBasicBlock = new BasicBlock();
    currentBasicBlock->add_successor(endIfBasicBlock);

    // then create a new basic block for the THEN control flow, which sets a new current basic block
    startNewBasicBlockSuccessor();

    // process the THEN control flow in the new current basic block
    node->getChildren()[1]->accept(this);

    // last basic block of the "then" branch, will contain the ENDTHEN instruction
    // connect it to the ENDIF basic block
    currentBasicBlock->add_successor(endIfBasicBlock);

    // now set the current basic block to the ENDIF basic block and process from there
    currentBasicBlock = endIfBasicBlock;
    node->getChildren()[2]->accept(this);

    return "";
}

std::string IrFlowgraphVisitor::visitIfElseNode(IfElseNode *node)
{
    // add the IF instruction to the current basic block
    currentBasicBlock->add_instruction(node);

    // create a new basic block for the ENDIF instruction to be filled in later
    BasicBlock *endIfBasicBlock = new BasicBlock();

    // process the condition until TEST- keep adding this to the current basic block
    node->getChildren()[0]->accept(this);

    // then create a new basic block for the THEN control flow
    BasicBlock *savedCurrentBasicBlock = currentBasicBlock;
    startNewBasicBlockSuccessor();

    // process the THEN control flow
    node->getChildren()[1]->accept(this);

    // the last basic block of the "then" branch, will contain the ENDTHEN instruction
    // connect it to the ENDIF basic block
    currentBasicBlock->add_successor(endIfBasicBlock);

    // process the ELSE control flow, first set the current basic block back to the saved basic block with the TEST instruction
    currentBasicBlock = savedCurrentBasicBlock;
    startNewBasicBlockSuccessor();
    node->getChildren()[2]->accept(this);

    // the last basic block of the "else" branch, will contain the ENDELSE instruction
    // connect it to the ENDIF basic block
    currentBasicBlock->add_successor(endIfBasicBlock);

    // now set the current basic block to the ENDIF basic block and process from there
    currentBasicBlock = endIfBasicBlock;
    node->getChildren()[3]->accept(this);

    return "";
}

void IrFlowgraphVisitor::startNewBasicBlockSuccessor()
{

    // create a new basic block and add it as a successor to the current basic block
    BasicBlock *newBasicBlock = new BasicBlock();
    currentBasicBlock->add_successor(newBasicBlock);
    currentBasicBlock = newBasicBlock;
}