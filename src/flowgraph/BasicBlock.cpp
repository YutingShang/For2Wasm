#include "BasicBlock.h"

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

// std::vector<BaseNode*> BasicBlock::get_instructions() {
//     return instructions;
// }


