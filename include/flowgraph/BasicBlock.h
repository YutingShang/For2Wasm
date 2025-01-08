#pragma once

#include <vector>
#include <list>
#include <string>
#include <iterator>
#include "BaseNode.h"
#include "SimpleNode.h"

class BasicBlock {

    public:

        BasicBlock();

        void add_instruction(BaseNode* instruction);

        void add_successor(BasicBlock* successor);

        std::string getText();

        std::vector<BasicBlock*> get_successors();
        std::vector<BasicBlock*> get_predecessors();

        ///WARNING: returns reference for modification of the instructions list
        std::list<BaseNode*>& get_instructions_reference();
        const std::list<BaseNode*> get_instructions_copy();     ///if I want a copy, here is a with a const version

        // removes an instruction from the basic block (a doubly linked list)
        // takes an iterator pointing to the instruction to remove, and returns a new iterator to the next instruction
        // useful for removing instructions from the basic block in a loop
        /// NOTE: this will DELETE the instruction node, not just from the basic block, but also the ir tree
        std::list<BaseNode*>::iterator remove_instruction_node(std::list<BaseNode*>::iterator it);
        
        //extra wrapper functions around the SimpleNode insertSandwichChild method and replaceCurrentNodeInIRTree method
        //to handle the instructions list of the basic block, as well as the ir tree underneath
        ///NOTE: these will return a new iterator to the instruction AFTER the replaced/inserted instruction 
        std::list<BaseNode*>::iterator replace_instruction_node(std::list<BaseNode*>::iterator it, SimpleNode* newNode);
        std::list<BaseNode*>::iterator insert_sandwich_instruction_node(std::list<BaseNode*>::iterator it, SimpleNode* newNode);


    private:
        
        //NOTE: using a (doubly linked) list instead of a vector to allow for easy removal of instructions
        std::list<BaseNode*> instructions;

        std::vector<BasicBlock*> successors;
        std::vector<BasicBlock*> predecessors;      //needed for forward dataflow analysis

};