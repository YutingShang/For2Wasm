#include "AVAIL_PRE.h"
#include "VBE.h"

AVAIL_PRE::AVAIL_PRE(std::shared_ptr<BasicBlock> entryBasicBlock) : BaseDataFlowAnalysis<std::set<std::string>>(entryBasicBlock, AnalysisDirection::FORWARD) {
    std::shared_ptr<BaseNode> rootNode = entryBasicBlock->get_instructions_copy().front().lock();
    allExpressions = AnalysisTools::getAllProgramExpressions(rootNode);

    //get all anticipated (VBE) expressions in the program
    VBE vbe(entryBasicBlock);
    allNodesAnticipatedInExpressions = vbe.getNodeInDataFlowSets();

    //set the initial set to the universe of all expressions
    setInitialSet(allExpressions);

    //compute the dataflow sets for each basic block and instruction node
    computeDataFlowSets();
}

void AVAIL_PRE::printBlockDataFlowSets() {
    for (int i = 0; i < basicBlocks.size(); i++)
    {
        std::set<std::string> basicBlockAvailSet = blockDataFlowSets[i];
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Available Expressions (for PRE) Out-Sets: \n {";
        
        for (const auto &availExpr : basicBlockAvailSet)
        { // get the dataflow set for the basic block
            std::cout << availExpr << ", ";
        }
        std::cout << "}\n-------------------------\n"
                  << std::endl;
    }
}


//meet operation for AVAIL_PRE - returns the intersection of two sets
std::set<std::string> AVAIL_PRE::meetOperation(const std::set<std::string>& current_pre_avail_set, const std::set<std::string>& predecessor_pre_avail_set) {
    std::set<std::string> new_pre_avail_set;
    //take intersection of the two sets by inserting all elements from both sets into the new set
    std::set_intersection(current_pre_avail_set.begin(), current_pre_avail_set.end(), predecessor_pre_avail_set.begin(), predecessor_pre_avail_set.end(), std::inserter(new_pre_avail_set, new_pre_avail_set.begin()));
    return new_pre_avail_set;
}

//transfer function for AVAIL_PRE - returns the set of variables that are available at the after processing the current instruction (going downwards)
std::set<std::string> AVAIL_PRE::transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& in_pre_avail_set) {
    std::set<std::string> out_pre_avail_set = in_pre_avail_set;

    //get the anticipated[B].in and killed expressions from the instruction
    std::set<std::string> anticipatedExpressions = allNodesAnticipatedInExpressions[instruction];
    std::set<std::string> killedExpressions = AnalysisTools::getKilledExpressionsAtNode(instruction, allExpressions);

    //add the anticipated expressions to the AVAIL_PRE set
    out_pre_avail_set.insert(anticipatedExpressions.begin(), anticipatedExpressions.end());

    //remove the killed expressions from the AVAIL_PRE set
    for (std::string expr : killedExpressions) {
        out_pre_avail_set.erase(expr);
    }

    return out_pre_avail_set;
}

