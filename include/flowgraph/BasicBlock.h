#pragma once

#include <vector>
#include <list>
#include <string>
#include <iterator>
#include "BaseNode.h"
#include "SimpleNode.h"
class InsertableBasicBlock;

class BasicBlock {

    public:

        BasicBlock();

        virtual ~BasicBlock() = default;
        void delete_entire_flowgraph();

        void add_instruction(const std::shared_ptr<BaseNode>& instruction);

        //adds a successor to the basic block (and the corresponding predecessor to the successor), either 
        //1. always add the successor  (e.g. to connect the exit node to the endloop basic block) (checkIfCurrentIsExit = false)
        //2. only add the successor if the current basic block does not contain an exit node (DEFAULT, set checkIfCurrentIsExit to true)
        void add_successor(BasicBlock* successor, bool checkIfCurrentIsExit = true);

        //removes a successor from the basic block, and the corresponding predecessor from the successor
        void remove_successor(BasicBlock* successor);

        std::string getText();

        std::vector<BasicBlock*> get_successors();
        std::vector<BasicBlock*> get_predecessors();

        ///WARNING: returns reference for modification of the instructions list
        std::list<std::weak_ptr<BaseNode>>& get_instructions_reference();
        const std::list<std::weak_ptr<BaseNode>> get_instructions_copy();     ///if I want a copy, here is a with a const version

        // removes an instruction from the basic block (a doubly linked list)
        // takes an iterator pointing to the instruction to remove, and returns a new iterator to the next instruction
        // useful for removing instructions from the basic block in a loop
        /// NOTE: this will DELETE the instruction node, not just from the basic block, but also the ir tree
        std::list<std::weak_ptr<BaseNode>>::iterator remove_instruction_node(std::list<std::weak_ptr<BaseNode>>::iterator it);
        
        //extra wrapper functions around the SimpleNode insertSandwichChild method and replaceCurrentNodeInIRTree method
        //to handle the instructions list of the basic block, as well as the ir tree underneath
        ///NOTE: these will return a new iterator to the instruction AFTER the replaced/inserted instruction 
        std::list<std::weak_ptr<BaseNode>>::iterator replace_instruction_node(std::list<std::weak_ptr<BaseNode>>::iterator it, std::shared_ptr<SimpleNode> newNode);
        ///WARNING: pay attention to the order of the instructions list (reversed lists and reversed iterators may behave differently, to the assumed normal order)
        std::list<std::weak_ptr<BaseNode>>::iterator insert_sandwich_instruction_node(std::list<std::weak_ptr<BaseNode>>::iterator it, std::shared_ptr<SimpleNode> newNode, bool instructionsListReversed);

        //calls the insertSandwichParent method on the instruction node at the iterator
        //iterator is still valid after this, since insertion is before the iterator
        void insert_parent_instruction_node(std::list<std::weak_ptr<BaseNode>>::iterator it, std::shared_ptr<SimpleNode> newNode);

        //insert a sandwich Basic block between this basic block and its specific predecessor
        void insert_sandwich_predecessor_basic_block(BasicBlock* currentPredecessor, InsertableBasicBlock* newBasicBlock);

        void setContainsExitNode(bool containsExitNode);
    protected:
        
        //NOTE: using a (doubly linked) list instead of a vector to allow for easy removal of instructions
        std::list<std::weak_ptr<BaseNode>> instructions;

        std::vector<BasicBlock*> successors;
        std::vector<BasicBlock*> predecessors;      //needed for forward dataflow analysis

        bool currentBlockContainsExitNode = false;   //set to true if the current basic block contains an exit node
};