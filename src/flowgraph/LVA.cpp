#include "LVA.h"


//constructor for LVA - initialises the base class with the entry basic block, BACKWARD analysis direction, and an empty set
LVA::LVA(BasicBlock* entryBasicBlock) : 
    BaseDataFlowAnalysis<std::set<std::string>>(entryBasicBlock, AnalysisDirection::BACKWARD) {

    //set the initial set to an empty set
    setInitialSet(std::set<std::string>());

    //compute the dataflow sets for each basic block
    computeDataFlowSets();
}

void LVA::printBlockDataFlowSets() {
    for (int i = 0; i < basicBlocks.size(); i++)
    {
        std::set<std::string> basicBlockLiveSet = blockDataFlowSets[i];
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Live Variable In-Sets: \n {";
        
        for (const auto &liveVar : basicBlockLiveSet)
        { // get the dataflow set for the basic block
            std::cout << liveVar << ", ";
        }
        std::cout << "}\n-------------------------\n"
                  << std::endl;
    }
}


//meet operation for LVA - returns the union of two sets
std::set<std::string> LVA::meetOperation(const std::set<std::string>& current_live_set, const std::set<std::string>& successor_live_set) {
    std::set<std::string> new_live_set;
    //take union of the two sets by inserting all elements from both sets into the new set
    new_live_set.insert(current_live_set.begin(), current_live_set.end());
    new_live_set.insert(successor_live_set.begin(), successor_live_set.end());
    return new_live_set;
}

//transfer function for LVA - returns the set of variables that are live after processing the current instruction (going upwards)
std::set<std::string> LVA::transferFunction(BaseNode* instruction, const std::set<std::string>& out_live_set) {
    //copy the dataflow set to a new set
    std::set<std::string> in_live_set = out_live_set;

    //get the def and ref sets from the instruction
    std::set<std::string> def_set = instruction->getDefinedVariables();
    std::set<std::string> ref_set = instruction->getReferencedVariables();

    //remove the defs from the live set
    for (std::string var : def_set) {
        in_live_set.erase(var);
    }
    //add the refs to the live set
    in_live_set.insert(ref_set.begin(), ref_set.end());

    return in_live_set;
}

