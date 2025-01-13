#include "DeadCodeElimination.h"
#include "LVA.h"

void DeadCodeElimination::iterateDeadCodeElimination(BasicBlock *entryBasicBlock)
{
    bool runDCE = true;
    while (runDCE)
    {
        runDCE = deadCodeEliminationOnce(entryBasicBlock);
    }
}

bool DeadCodeElimination::deadCodeEliminationOnce(BasicBlock *entryBasicBlock)
{
    // Analysis: LVA
    LVA lva(entryBasicBlock);
    std::vector<BasicBlock *> basicBlocks = lva.getBasicBlocksUsed();
    std::vector<std::set<std::string>> liveSets = lva.getLiveSets();

    // Transformation: Remove dead code
    bool removed = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        // the live sets have already converged now, so we just pass through the iteration one more time
        // compute the live set for each instruction and check if any instructions are dead code
        removed |= basicBlockRemoveDeadCode(basicBlock, basicBlocks, liveSets);

    }

    return removed;
    /// TODO: handle case if an entire basic block is removed - now possible with the `-simplify` flag? Or just draw the flowgraph again?
}


bool DeadCodeElimination::basicBlockRemoveDeadCode(BasicBlock *basicBlock, std::vector<BasicBlock *> &basicBlocks, std::vector<std::set<std::string>> &liveSets)
{
    bool modified = false;
    // iterate through the instructions in the basic block, starting from the bottom
    // check the (out)live set of each instruction
    // if that instruction is an defines a variable, which is not in the (out)live set of the instruction
    // then it is dead code and we can remove it

    // outlive set of the basic block (or also for the last instruction)...
    //...is the union of the (in)live set of all the successors of the basic block
    std::set<std::string> out_live_set;

    for (BasicBlock *successor : basicBlock->get_successors())
    {
        // find the index of the successor in the basicBlocks vector
        int successor_index = std::find(basicBlocks.begin(), basicBlocks.end(), successor) - basicBlocks.begin();
        // get the live set of the successor
        // live[s]
        std::set<std::string> successor_live_set = liveSets[successor_index];
        out_live_set.insert(successor_live_set.begin(), successor_live_set.end());
    }

    // then we process all the instructions in the basic block linearly upwards

    // gets a REFERENCE of the instructions
    ///NOTE: modifies the instructions directly with remove_instruction_node method
    std::list<BaseNode *>& instructions = basicBlock->get_instructions_reference(); 
    instructions.reverse();              // reverse the instructions to iterate from the bottom up (remove_instruction_node needs a normal iterator it)
    for (auto it = instructions.begin(); it != instructions.end();)
    {
        // for each instruction, the (out)live set is stored in the current out_live_set variable

        BaseNode *instruction = *it;
        std::set<std::string> def_set = instruction->getDefinedVariables();
        std::set<std::string> ref_set = instruction->getReferencedVariables();

        if (!def_set.empty())
        { // if the instruction does assign/define a variable

            // check if the variable is NOT in the (out)live set
            /// NOTE: assumes only one variable defined per instruction
            assert(def_set.size() == 1);
            std::string var = *def_set.begin(); // gets iterator and dereferences it

            if (out_live_set.find(var) == out_live_set.end())
            { // variable is not live after this instruction
                // this instruction is dead code and we can remove it - from the instruction list and also the ir tree
                // don't increment the iterator, since a new one is returned
                it = basicBlock->remove_instruction_node(it);
                modified = true;
            }
            else
            {
                // the instruction is not dead code and we can move to the next instruction
                ++it;
            }
        }
        else
        {
            // the instruction does not define a variable
            ++it;
        }

        // calculate the new (out)live set
        for (std::string var : def_set)
        {
            out_live_set.erase(var);
        }

        for (std::string var : ref_set)
        {
            out_live_set.insert(var);
        }
    }

    ///NOTE: reversing the instructions again to restore the original order
    instructions.reverse();

    return modified;
}