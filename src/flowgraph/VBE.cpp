#include "VBE.h"

VBE::VBE(BasicBlock* entryBasicBlock) : BaseDataFlowAnalysis(entryBasicBlock, AnalysisDirection::BACKWARD) {
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    allExpressions = AnalysisTools::getAllProgramExpressions(rootNode);

    //set the initial set to the universe of all expressions
    setInitialSet(allExpressions);

    //compute the dataflow sets for each basic block and instruction node
    computeDataFlowSets();
}

void VBE::printBlockDataFlowSets() {

        for (int i = 0; i < basicBlocks.size(); i++)
    {
        std::set<std::string> basicBlockAvailSet = blockDataFlowSets[i];
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Very Busy Expressions In-Sets: \n {";
        
        for (const auto &availExpr : basicBlockAvailSet)
        { // get the dataflow set for the basic block
            std::cout << availExpr << ", ";
        }
        std::cout << "}\n-------------------------\n"
                  << std::endl;
    }

}

std::set<std::string> VBE::meetOperation(const std::set<std::string>& current_vbe_set, const std::set<std::string>& successor_vbe_set) {
    std::set<std::string> result;
    std::set_intersection(current_vbe_set.begin(), current_vbe_set.end(), successor_vbe_set.begin(), successor_vbe_set.end(), std::inserter(result, result.begin()));
    return result;
}

std::set<std::string> VBE::transferFunction(BaseNode* instruction, const std::set<std::string>& out_vbe_set) {
    std::set<std::string> in_vbe_set = out_vbe_set;

    //get the generated and killed expressions from the instruction
    std::set<std::string> usedExpressions = instruction->getReferencedExpressions();
    std::set<std::string> killedExpressions = AnalysisTools::getKilledExpressionsAtNode(instruction, allExpressions);

    //remove the killed expressions from the in-set
    for (std::string expr : killedExpressions) {
        in_vbe_set.erase(expr);
    }

    //add the used expressions to the in-set
    in_vbe_set.insert(usedExpressions.begin(), usedExpressions.end());

    return in_vbe_set;
}