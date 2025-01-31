#include "BasicBlock.h"
#include <iterator>
#include <iostream>
#include <stack>
#include <set>
#include "InsertableBasicBlock.h"


BasicBlock::BasicBlock() {}

void BasicBlock::delete_entire_flowgraph() {
    
    std::stack<BasicBlock*> toExploreStack;
    toExploreStack.push(this);
    std::set<BasicBlock*> seenBlocks;
    seenBlocks.insert(this);

    //get a list of all the basic blocks using DFS
    while (!toExploreStack.empty()) {
        BasicBlock *current = toExploreStack.top();
        toExploreStack.pop();

        for (BasicBlock* successor : current->successors) {
            if(seenBlocks.find(successor) == seenBlocks.end()) {
                toExploreStack.push(successor);
                seenBlocks.insert(successor);
            }
        }
    }

    //delete all the basic blocks
    for (BasicBlock* bb: seenBlocks) {
        delete bb;
    }
}

void BasicBlock::add_instruction(BaseNode* instruction) {
    instructions.push_back(instruction);
}

std::string BasicBlock::getText() {
    std::string basicBlock = "";
    for (BaseNode* instruction : instructions) {
        basicBlock += instruction->getText() + "\n";
    }
    return basicBlock;
}

void BasicBlock::add_successor(BasicBlock* successor, bool checkIfCurrentIsExit) {
    if (checkIfCurrentIsExit && currentBlockContainsExitNode) {   //if the current basic block contains an exit node AND we are told to check, don't add the successor
        return;
    }
    //otherwise, add the successor and the corresponding predecessor
    successors.push_back(successor);
    successor->predecessors.push_back(this);
}

void BasicBlock::remove_successor(BasicBlock* successor) {
    //remove shifts the element to remove to the end of the vector, and erase actually removes the element from the end of the vector
    successors.erase(std::remove(successors.begin(), successors.end(), successor), successors.end());
    successor->predecessors.erase(std::remove(successor->predecessors.begin(), successor->predecessors.end(), this), successor->predecessors.end());
}

void BasicBlock::setContainsExitNode(bool containsExitNode) {
    currentBlockContainsExitNode = containsExitNode;
}


std::vector<BasicBlock*> BasicBlock::get_successors() {
    return successors;
}

std::vector<BasicBlock*> BasicBlock::get_predecessors() {
    return predecessors;
}

std::list<BaseNode*>& BasicBlock::get_instructions_reference() {
    return instructions;
}

const std::list<BaseNode*> BasicBlock::get_instructions_copy() {
    return instructions;
}



std::list<BaseNode*>::iterator BasicBlock::remove_instruction_node(std::list<BaseNode*>::iterator it) {
    //get what instruction is being removed
    BaseNode* instruction = *it;

    //remove the instruction from the basic block
    it = instructions.erase(it);

    //edit the ir tree to remove the instruction node 
    //bypass the current node, add its children to the parent node
    instruction->removeCurrentNodeFromIRTree();

    // return the iterator to the next instruction
    return it;
}


std::list<BaseNode*>::iterator BasicBlock::replace_instruction_node(std::list<BaseNode*>::iterator it, SimpleNode* newNode) {
    //get what instruction is being replaced
    BaseNode* instruction = *it;

    // handle the instructions list of the basic block
    ///NOTE: this is completely separate from the IR Tree
    it = instructions.erase(it);       //iterator now points to the next instruction node
    instructions.insert(it, newNode);   //inserts newNode before the next element, iterator still points this next element

    //replace the instruction with the new node, in the IR Tree
    SimpleNode* simpleNode = dynamic_cast<SimpleNode*>(instruction);
    if (simpleNode == nullptr) {
        throw std::runtime_error("ERROR when replacing instruction node: Instruction is not a SimpleNode subclass");
    }
   
    simpleNode->replaceCurrentNodeInIRTree(newNode);

    //return the iterator to the next instruction (after the newNode)
    return it;
}

std::list<BaseNode*>::iterator BasicBlock::insert_sandwich_instruction_node(std::list<BaseNode*>::iterator it, SimpleNode* newNode, bool instructionsListReversed) {

    //get what instruction is being replaced
    BaseNode* instruction = *it;

    //replace the instruction with the new node, in the IR Tree
    SimpleNode* simpleNode = dynamic_cast<SimpleNode*>(instruction);
    if (simpleNode == nullptr) {
        throw std::runtime_error("ERROR when inserting sandwich instruction node: Instruction is not a SimpleNode subclass");
    }
    simpleNode->insertSandwichChild(newNode);

    // handle the instructions list of the basic block
    ///NOTE: this is completely separate from the IR Tree
    if (instructionsListReversed) {
        //it;  don't change the iterator, to insert the newNode 'before' the current instruction (when in reverse), appearing right after the current instruction in the actual basic block
    } else {
        ++it;     //increment iterator to point to the next instruction node, to insert the newNode before it, to appear right after the current instruction in the actual basic block
    }
    instructions.insert(it, newNode);   //iterator will point to the next instruction after the newNode

    //return the iterator to the next instruction (after the newNode)
    return it;
}

void BasicBlock::insert_parent_instruction_node(std::list<BaseNode*>::iterator it, SimpleNode* newNode) {
    BaseNode* instruction = *it;
    instruction->insertSandwichParent(newNode);
    instructions.insert(it, newNode);   //iterator will still point to the same instruction
}

void BasicBlock::insert_sandwich_predecessor_basic_block(BasicBlock* currentPredecessor, InsertableBasicBlock* newBasicBlock) {
    //insert the new basic block as a predecessor of this basic block
    //between the current predecessor and this basic block

    //first remove all connections between the current predecessor and this basic block
    currentPredecessor->remove_successor(this);

    //connect the new basic block in between
    currentPredecessor->add_successor(newBasicBlock, false);     //always connects the new basic block to the current predecessor
    newBasicBlock->add_successor(this, false);     //always connects the new basic block to this basic block

}






