#include "AVAIL.h"


AVAIL::AVAIL(BasicBlock* entryBasicBlock) : BaseDataFlowAnalysis<std::set<std::string>>(entryBasicBlock, AnalysisDirection::FORWARD) {
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    allExpressions = AnalysisTools::getAllProgramExpressions(rootNode);

    //set the initial set to the universe of all expressions
    setInitialSet(allExpressions);

    //compute the dataflow sets for each basic block and instruction node
    computeDataFlowSets();
}


//meet operation for AVAIL - returns the intersection of two sets
std::set<std::string> AVAIL::meetOperation(const std::set<std::string>& current_avail_set, const std::set<std::string>& predecessor_avail_set) {
    std::set<std::string> new_avail_set;
    //take intersection of the two sets by inserting all elements from both sets into the new set
    std::set_intersection(current_avail_set.begin(), current_avail_set.end(), predecessor_avail_set.begin(), predecessor_avail_set.end(), std::inserter(new_avail_set, new_avail_set.begin()));
    return new_avail_set;
}

//transfer function for AVAIL - returns the set of variables that are available at the after processing the current instruction (going downwards)
std::set<std::string> AVAIL::transferFunction(BaseNode* instruction, const std::set<std::string>& in_avail_set) {
    std::set<std::string> out_avail_set = in_avail_set;

    //get the generated and killed expressions from the instruction
    std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();
    std::set<std::string> killedExpressions = AnalysisTools::getKilledExpressionsAtNode(instruction, allExpressions);

    //remove the killed expressions from the avail set
    for (std::string expr : killedExpressions) {
        out_avail_set.erase(expr);
    }

    //add the generated expressions to the avail set
    out_avail_set.insert(generatedExpressions.begin(), generatedExpressions.end());

    return out_avail_set;
}

