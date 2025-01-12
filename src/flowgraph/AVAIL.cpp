#include "AVAIL.h"

AVAIL::AVAIL(BasicBlock* entryBasicBlock) : entryBasicBlock(entryBasicBlock) {
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    allExpressions = AnalysisTools::getAllProgramExpressions(rootNode);
    computeAvailSets();
}

std::vector<std::set<std::string>> AVAIL::getAvailSets() {
    return availSets;
}

std::vector<BasicBlock*> AVAIL::getBasicBlocks() {
    return basicBlocks;
}

std::set<std::string> AVAIL::getAllExpressions() {
    return allExpressions;
}

void AVAIL::computeAvailSets() {

    ///NOTE: this is the AVAIL algorithm
    // have a vector of avail sets, one for each basic block
    // for entry (instruction!) node, avail set is initialised to empty (not the entire basic block) - we will check for this in basicBlockComputeAvailSet
    // for all node, initialise to universe of all expressions
    availSets = std::vector<std::set<std::string>>(basicBlocks.size(), allExpressions);

    //iterate until no more changes
    bool changed = true;

    while (changed) {
        changed = false;
        for (int i = 0; i < basicBlocks.size(); i++) {     //still start from 0, process all basic blocks
            std::set<std::string> new_avail_set = basicBlockComputeAvailSet(basicBlocks[i]);
            if (new_avail_set != availSets[i]) {      //check if the avail set has changed, if so, update the avail set and set changed to true
                availSets[i] = new_avail_set;
                changed = true;
            }
        }
    }
}

std::set<std::string> AVAIL::basicBlockComputeAvailSet(BasicBlock* basicBlock) {
    //compute the avail set for the basic block
    ///NEW: avail[i] = (∩ avail[p]) - killed(i) U gen(i), for all predecessors p of i
    //the avail[] stored, is actually the out-avail

    std::set<std::string> availSet = allExpressions;     //initialise to universe of all expressions, to get intersection of all predecessors in a pairwise manner
   
    //in-avail for the first instruction is the intersection of the out-avail of all the predecessors
    //this intersection is empty if there are no predecessors
    std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
    if (predecessors.empty()) {
        availSet = {};
    } else {
        for (BasicBlock* predecessor : predecessors) {
   
            //get the avail[p] for the predecessor
            int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
            std::set<std::string> predecessorAvailSet = availSets[predecessorIndex];

            //intersect the predecessor avail set with the avail set, store in availSet
            std::set<std::string> intersectedAvailSet;
            std::set_intersection(availSet.begin(), availSet.end(), predecessorAvailSet.begin(), predecessorAvailSet.end(), std::inserter(intersectedAvailSet, intersectedAvailSet.begin()));
            availSet = intersectedAvailSet;
        }
    } 
    //availSet now temporarily stores the in-avail set of the basic block/first instruction (∩ avail[p])

    //then we process all the instructions in the basic block linearly downwards
    //so that availSet computes and stores the out-avail for each instruction node
    const std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();     //gets reference
    for (auto it = instructions.begin(); it != instructions.end(); ++it) {
        
        BaseNode* instruction = *it;
        std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();
        std::set<std::string> killedExpressions = AnalysisTools::getKilledProgramExpressions(instruction, allExpressions);

        //remove the killed expressions from the avail set
        for (std::string expr : killedExpressions) {
            availSet.erase(expr);
        }

        //add the generated expressions to the avail set
        for (std::string expr : generatedExpressions) {
            availSet.insert(expr);
        }
    }

    //the availSet now stores the out-avail set of the basic block/last instruction
    return availSet;
} 