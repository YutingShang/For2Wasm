#include "ACS.h"
#include <iostream>

ACS::ACS(BasicBlock* entryBasicBlock): BaseDataFlowAnalysis<std::set<std::pair<std::string, std::string>>>(entryBasicBlock, AnalysisDirection::FORWARD) {
    std::shared_ptr<BaseNode> rootNode = entryBasicBlock->get_instructions_copy().front().lock();
    allCopyStatements = AnalysisTools::getAllProgramCopyStatements(rootNode);

    //set the initial set to the universe of all copy statements
    setInitialSet(allCopyStatements);

    //compute the dataflow sets for each basic block and instruction node
    computeDataFlowSets();
}

void ACS::printBlockDataFlowSets() {

    for (int i = 0; i < basicBlocks.size(); i++) {
        std::set<std::pair<std::string, std::string>> basicBlockAvailCopies = blockDataFlowSets[i];
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Available copy statements: \n {";
        for (const auto& copyStatement : basicBlockAvailCopies) {
            std::cout << copyStatement.first << " -> " << copyStatement.second << ", ";
        }
        std::cout << "}\n-------------------------\n" << std::endl;
    }
}

//meet operation for ACS - returns the intersection of two sets
std::set<std::pair<std::string, std::string>> ACS::meetOperation(const std::set<std::pair<std::string, std::string>>& current_availCopies_set, const std::set<std::pair<std::string, std::string>>& predecessor_availCopies_set) {
    std::set<std::pair<std::string, std::string>> new_availCopies_set;
    //take intersection of the two sets
    std::set_intersection(current_availCopies_set.begin(), current_availCopies_set.end(), predecessor_availCopies_set.begin(), predecessor_availCopies_set.end(), std::inserter(new_availCopies_set, new_availCopies_set.begin()));
    return new_availCopies_set;
}

//transfer function for ACS - returns the set of variables that are available at the after processing the current instruction (going forwards)
std::set<std::pair<std::string, std::string>> ACS::transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::pair<std::string, std::string>>& in_availCopies_set) {
    std::set<std::pair<std::string, std::string>> out_availCopies_set = in_availCopies_set;

    //get the generated and killed expressions from the instruction
    std::set<std::pair<std::string, std::string>> generatedCopyStatements = AnalysisTools::getGeneratedCopyStatementsAtNode(instruction);
    std::set<std::pair<std::string, std::string>> killedCopyStatements = AnalysisTools::getKilledCopyStatementsAtNode(instruction, allCopyStatements);

    //remove the killed copy statements from the avail set
    for (std::pair<std::string, std::string> copyStatement : killedCopyStatements) {
        out_availCopies_set.erase(copyStatement);
    }

    //add the generated expressions to the avail set
    out_availCopies_set.insert(generatedCopyStatements.begin(), generatedCopyStatements.end());

    return out_availCopies_set;
}

