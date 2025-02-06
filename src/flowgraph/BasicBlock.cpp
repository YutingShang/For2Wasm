#include "BasicBlock.h"
#include <iostream>
#include <stack>
#include <set>
#include "InsertableBasicBlock.h"


BasicBlock::BasicBlock() {}

//getters

std::string BasicBlock::getText() {
    std::string basicBlock = "";
    for (std::weak_ptr<BaseNode> instruction : instructions) {
        basicBlock += instruction.lock()->getText() + "\n";
    }
    return basicBlock;
}

std::vector<std::shared_ptr<BasicBlock>> BasicBlock::get_successors() {
    return successors;
}

std::vector<std::weak_ptr<BasicBlock>> BasicBlock::get_predecessors() {
    return predecessors;
}


const std::list<std::weak_ptr<BaseNode>> BasicBlock::get_instructions_copy() {
    return instructions;
}

//setters

void BasicBlock::setContainsExitNode(bool containsExitNode) {
    currentBlockContainsExitNode = containsExitNode;
}

//flowgraph manipulation methods

void BasicBlock::add_instruction(const std::shared_ptr<BaseNode>& instruction) {
    instructions.push_back(instruction);
}

void BasicBlock::add_successor(const std::shared_ptr<BasicBlock>& successor, bool checkIfCurrentIsExit) {
    if (checkIfCurrentIsExit && currentBlockContainsExitNode) {   //if the current basic block contains an exit node AND we are told to check, don't add the successor
        return;
    }
    //otherwise, add the successor and the corresponding predecessor
    successors.push_back(successor);
    successor->predecessors.push_back(shared_from_this());
}

void BasicBlock::remove_successor(const std::shared_ptr<BasicBlock>& successor) {
    //remove shifts the element to remove to the end of the vector, and erase actually removes the element from the end of the vector
    successors.erase(std::remove(successors.begin(), successors.end(), successor), successors.end());
    
    // Remove this block from successor's predecessors
    auto& pred = successor->predecessors;
    pred.erase(
        std::remove_if(pred.begin(), pred.end(),
            [this](const std::weak_ptr<BasicBlock>& predBlock) {
                return predBlock.lock().get() == this;
            }
        ),
        pred.end()
    );
}

void BasicBlock::insert_sandwich_predecessor_basic_block(const std::shared_ptr<BasicBlock>& currentPredecessor, std::shared_ptr<InsertableBasicBlock>& newBasicBlock) {
    //insert the new basic block as a predecessor of this basic block
    //between the current predecessor and this basic block

    //first remove all connections between the current predecessor and this basic block
    currentPredecessor->remove_successor(shared_from_this());

    //connect the new basic block in between
    currentPredecessor->add_successor(std::shared_ptr<BasicBlock>(newBasicBlock), false);     //always connects the new basic block to the current predecessor
    newBasicBlock->add_successor(shared_from_this(), false);     //always connects the new basic block to this basic block
}






