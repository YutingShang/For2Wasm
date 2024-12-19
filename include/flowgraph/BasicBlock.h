#pragma once

#include <vector>
#include <set>
#include <string>
#include "BaseNode.h"

class BasicBlock {

    public:

        BasicBlock();

        void add_instruction(BaseNode* instruction);

        void add_successor(BasicBlock* successor);

        void add_ref(std::string ref);

        void add_def(std::string def);

        void add_live(std::string live);

        std::vector<BaseNode*> get_instructions();

        std::vector<BasicBlock*> get_successors();

        std::set<std::string> get_ref_set();

        std::set<std::string> get_def_set();

        std::set<std::string> get_live_set();

        std::string getText();

        // std::string stringifyBasicBlock();


    private:
        
        std::vector<BaseNode*> instructions;

        std::vector<BasicBlock*> successors;
        // std::vector<BasicBlock*> predecessors;    ///NOTE: is this needed?

        std::set<std::string> ref_set;

        std::set<std::string> def_set;

        std::set<std::string> live_set;


};