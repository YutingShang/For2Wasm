#include "LVA.h"

LVA::LVA(BasicBlock* entryBasicBlock) : entryBasicBlock(entryBasicBlock) {
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);
    std::reverse(basicBlocks.begin(), basicBlocks.end());  // reverse for backwards analysis
    computeLiveSets();
}

std::vector<std::set<std::string>> LVA::getLiveSets() {
    return liveSets;
}

std::vector<BasicBlock*> LVA::getBasicBlocksUsed() {
    return basicBlocks;
}

void LVA::computeLiveSets() {

    ///NOTE: this is the LVA algorithm
    // have a vector of live sets, one for each basic block, initialised to empty sets
    // for i=1 to n do live[i] := {}
    liveSets = std::vector<std::set<std::string>>(basicBlocks.size(), std::set<std::string>());

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < basicBlocks.size(); i++) {
            std::set<std::string> new_live_set = basicBlockComputeLiveSet(basicBlocks[i]);
            if (new_live_set != liveSets[i]) {
                liveSets[i] = new_live_set;
                changed = true;
            }
        }
    }
}

std::set<std::string> LVA::basicBlockComputeLiveSet(BasicBlock* basicBlock) {
    // outlive set of the basic block (or also for the last instruction)...
    //...is the union of the (in)live set of all the successors of the basic block
    std::set<std::string> live_set;

    for (BasicBlock* successor : basicBlock->get_successors()) {
        // find the index of the successor in the basicBlocks vector
        int successor_index = std::find(basicBlocks.begin(), basicBlocks.end(), successor) - basicBlocks.begin();
        // get the live set of the successor
        // live[s]
        std::set<std::string> successor_live_set = liveSets[successor_index];
        live_set.insert(successor_live_set.begin(), successor_live_set.end());
    }

    // then we process all the instructions in the basic block linearly upwards/backwards
    // first remove the def, then add the ref
    const auto& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
        BaseNode* instruction = *it;
        std::set<std::string> def_set = instruction->getDefinedVariables();
        std::set<std::string> ref_set = instruction->getReferencedVariables();

        // remove the defined vars from the live set
        for (std::string var : def_set) {
            live_set.erase(var);
        }

        // add the referenced vars to the live set
        for (std::string var : ref_set) {
            live_set.insert(var);
        }
    }

    // the result should be the live set of the first instruction of the basic block
    // this is also the (in)live set of the basic block
    return live_set;
} 


