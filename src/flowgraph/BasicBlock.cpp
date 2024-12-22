#include "BasicBlock.h"
#include <iterator>
#include <iostream>

BasicBlock::BasicBlock() {

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

void BasicBlock::add_successor(BasicBlock* successor) {
    successors.push_back(successor);
}

std::vector<BasicBlock*> BasicBlock::get_successors() {
    return successors;
}

std::list<BaseNode*>& BasicBlock::get_instructions() {
    return instructions;
}


std::list<BaseNode*>::iterator BasicBlock::remove_instruction_node(std::list<BaseNode*>::iterator it) {
    //get what instruction is being removed
    BaseNode* instruction = *it;

    //remove the instruction from the basic block
    it = instructions.erase(it);

    BaseNode* parent = instruction->getParent();
    if (parent == nullptr) {
        throw std::runtime_error("Parent node is null. Why are you removing this node?");
    }
    const auto parentChildren = parent->getChildren();
 

    //get the index of the current instruction in the parent node as an iterator
    auto instructionIterator = std::find(parentChildren.begin(), parentChildren.end(), instruction);
    auto instructionIndex = std::distance(parentChildren.begin(), instructionIterator);

    //edit the ir tree to remove the instruction node 
    //bypass the current node, add its children to the parent node
    std::vector<BaseNode*> currentChildren = instruction->getChildren();
    parent->removeChild(instruction);    //remove the current instruction from the parent node to make space

    for (BaseNode* child : currentChildren) {
        child->setParent(parent);
        parent->insertChild(child, instructionIndex);
        instructionIndex++;
    }

    // delete the instruction node from memory
    delete instruction;

    // return the iterator to the next instruction
    return it;
}





