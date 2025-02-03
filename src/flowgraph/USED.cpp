#include "USED.h"

USED::USED(BasicBlock* entryBasicBlock) : BaseDataFlowAnalysis(entryBasicBlock, AnalysisDirection::BACKWARD) {
    //get the 'latest expressions' for all nodes
    std::shared_ptr<BaseNode> rootNode = entryBasicBlock->get_instructions_copy().front().lock();
    std::set<std::string> allExpressions = AnalysisTools::getAllProgramExpressions(rootNode);
    allNodesLatestExpressions = AnalysisTools::getAllNodesLatestExpressions(entryBasicBlock, allExpressions, basicBlocks);

    //set the initial set to the empty set
    setInitialSet(std::set<std::string>());

    //compute the dataflow sets for each basic block and instruction node
    computeDataFlowSets();
}

void USED::printBlockDataFlowSets() {

        for (int i = 0; i < basicBlocks.size(); i++)
    {
        std::set<std::string> basicBlockUsedSet = blockDataFlowSets[i];
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Used Expressions In-Sets: \n {";
        
        for (const auto &usedExpr : basicBlockUsedSet)
        { // get the dataflow set for the basic block
            std::cout << usedExpr << ", ";
        }
        std::cout << "}\n-------------------------\n"
                  << std::endl;
    }

}

std::set<std::string> USED::meetOperation(const std::set<std::string>& current_used_set, const std::set<std::string>& successor_used_set) {
    std::set<std::string> result;
    std::set_union(current_used_set.begin(), current_used_set.end(), successor_used_set.begin(), successor_used_set.end(), std::inserter(result, result.begin()));
    return result;
}

std::set<std::string> USED::transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& out_used_set) {
    std::set<std::string> in_used_set = out_used_set;

    //get the generated and killed expressions from the instruction
    std::set<std::string> usedExpressions = instruction->getReferencedExpressions();
    std::set<std::string> latestExpressions = allNodesLatestExpressions[instruction];

    //add the used expressions to the in-set
    in_used_set.insert(usedExpressions.begin(), usedExpressions.end());

    //remove the latest expressions from the in-set
    for (std::string expr : latestExpressions) {
        in_used_set.erase(expr);
    }

    return in_used_set;
}

std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> USED::getNodesLatestExpressionsSets() {
    return allNodesLatestExpressions;
}
