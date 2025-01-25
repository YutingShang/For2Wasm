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
    std::unordered_map<BaseNode*, std::set<std::string>> nodeLiveSets = lva.getNodeOutDataFlowSets();

    // Transformation: Remove dead code
    bool removed = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        // the live sets have already converged now, so we just pass through the iteration one more time
        // compute the live set for each instruction and check if any instructions are dead code
        removed |= basicBlockRemoveDeadCode(basicBlock, basicBlocks, nodeLiveSets);

    }

    return removed;
    /// TODO: handle case if an entire basic block is removed - now possible with the `-simplify` flag? Or just draw the flowgraph again?
}


bool DeadCodeElimination::basicBlockRemoveDeadCode(BasicBlock *basicBlock, std::vector<BasicBlock *> &basicBlocks, std::unordered_map<BaseNode*, std::set<std::string>> &nodeLiveSets)
{
    bool modified = false;
    // iterate through the instructions in the basic block, starting from the bottom
    // check the (out)live set of each instruction
    // if that instruction is an defines a variable, which is not in the (out)live set of the instruction
    // then it is dead code and we can remove it


    // gets a REFERENCE of the instructions
    ///NOTE: modifies the instructions directly with remove_instruction_node method
    std::list<BaseNode *>& instructions = basicBlock->get_instructions_reference(); 
    instructions.reverse();              // reverse the instructions to iterate from the bottom up (remove_instruction_node needs a normal iterator it)
    for (auto it = instructions.begin(); it != instructions.end();)
    {
        // for each instruction, the (out)live set is stored in the current out_live_set variable
        
        BaseNode *instruction = *it;
        std::set<std::string> out_live_set = nodeLiveSets[instruction];
        std::set<std::string> def_set = instruction->getDefinedVariables();

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
    }

    ///NOTE: reversing the instructions again to restore the original order
    instructions.reverse();

    return modified;
}