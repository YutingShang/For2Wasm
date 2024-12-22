#pragma once

#include <vector>
#include <list>
#include <string>
#include <iterator>
#include "BaseNode.h"

class BasicBlock {

    public:

        BasicBlock();

        void add_instruction(BaseNode* instruction);

        void add_successor(BasicBlock* successor);

        std::string getText();

        std::vector<BasicBlock*> get_successors();

        ///WARNING: returns reference for modification of the instructions list
        ///if I want a copy, overload get_instructions() with a const version
        std::list<BaseNode*>& get_instructions();

        // removes an instruction from the basic block (a doubly linked list)
        // takes an iterator pointing to the instruction to remove, and returns a new iterator to the next instruction
        // useful for removing instructions from the basic block in a loop
        /// NOTE: this will DELETE the instruction node, not just from the basic block, but also the ir tree
        std::list<BaseNode*>::iterator remove_instruction_node(std::list<BaseNode*>::iterator it);


    private:
        
        //NOTE: using a (doubly linked) list instead of a vector to allow for easy removal of instructions
        std::list<BaseNode*> instructions;

        std::vector<BasicBlock*> successors;

};