#pragma once

#include <vector>
#include <list>
#include <string>
#include "BaseNode.h"

class BasicBlock {

    public:

        BasicBlock();

        void add_instruction(BaseNode* instruction);

        void add_successor(BasicBlock* successor);

        std::string getText();

        std::vector<BasicBlock*> get_successors();

        std::list<BaseNode*> get_instructions();

        void set_instructions(std::list<BaseNode*> instructions);

 

        // std::set<std::string> get_live_set();

        // void compute_live_set();

     /// NOTE: should only be used with the dead code elimination
        // void remove_instruction();


    private:
        
        //NOTE: using a (doubly linked) list instead of a vector to allow for easy removal of instructions
        std::list<BaseNode*> instructions;

        std::vector<BasicBlock*> successors;

        // std::set<std::string> live_set;


};