#pragma once

#include <vector>
#include <list>
#include <string>
#include <memory>
#include "BaseNode.h"
#include "SimpleNode.h"
class InsertableBasicBlock;

class BasicBlock : public std::enable_shared_from_this<BasicBlock> {

    public:

        //constructors and destructors
        BasicBlock();
        virtual ~BasicBlock() = default;

        //getters 
        std::string getText();           //returns the text of the basic block
        const std::list<std::weak_ptr<BaseNode>> get_instructions_copy();          //returns a copy of the instructions list of pointers
        std::vector<std::shared_ptr<BasicBlock>> get_successors();
        std::vector<std::weak_ptr<BasicBlock>> get_predecessors();

        //setters and flowgraph manipulation methods
        void setContainsExitNode(bool containsExitNode);

        //adds an instruction to the basic block
        void add_instruction(const std::shared_ptr<BaseNode>& instruction);

        //adds a successor to the basic block (and the corresponding predecessor to the successor), either 
        //1. always add the successor  (e.g. to connect the exit node to the endloop basic block) (checkIfCurrentIsExit = false)
        //2. only add the successor if the current basic block does not contain an exit node (DEFAULT, set checkIfCurrentIsExit to true)
        void add_successor(const std::shared_ptr<BasicBlock>& successor, bool checkIfCurrentIsExit = true);

        //removes a successor from the basic block, and the corresponding predecessor from the successor
        void remove_successor(const std::shared_ptr<BasicBlock>& successor);

        //insert a sandwich Basic block between this basic block and its specific predecessor
        void insert_sandwich_predecessor_basic_block(const std::shared_ptr<BasicBlock>& currentPredecessor, std::shared_ptr<InsertableBasicBlock>& newBasicBlock);

    protected:
        
        //NOTE: using a (doubly linked) list instead of a vector to allow for easy removal of instructions
        std::list<std::weak_ptr<BaseNode>> instructions;

        std::vector<std::shared_ptr<BasicBlock>> successors;
        std::vector<std::weak_ptr<BasicBlock>> predecessors;      //needed for forward dataflow analysis

        bool currentBlockContainsExitNode = false;   //set to true if the current basic block contains an exit node
};