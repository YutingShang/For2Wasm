#include "ACS.h"
#include <iostream>

ACS::ACS(BasicBlock* entryBasicBlock): entryBasicBlock(entryBasicBlock) {
    //a forward analysis
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    allCopyStatements = AnalysisTools::getAllProgramCopyStatements(rootNode);
    computeAvailableCopySets();    //will initialise the reachingDefPoints vector
 }

std::vector<std::set<std::pair<std::string, std::string>>> ACS::getAvailCopies() {
    // printAvailCopies();
    return availCopies;
}

void ACS::printAvailCopies() {
        for (int i = 0; i < basicBlocks.size(); i++) {
            std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
            std::cout << "Available copy statements: " << std::endl;
            for (const auto& copyStatement : availCopies[i]) {
                std::cout << "Copy statement: " << copyStatement.first << " -> " << copyStatement.second << std::endl;
            }
        }
    std::cout << "-------------------------\n" << std::endl;
}

std::vector<BasicBlock*> ACS::getBasicBlocksUsed() {
    return basicBlocks;
}

std::set<std::pair<std::string, std::string>> ACS::getAllCopyStatements() {
    return allCopyStatements;
}

void ACS::computeAvailableCopySets() {

    //Available copy statements, forward analysis taking intersection of predecessors   

    //initialise the availCopies vector, each element is the universe of all copy statements
    availCopies = std::vector<std::set<std::pair<std::string, std::string>>>(basicBlocks.size(), allCopyStatements);

    //iterate until no more changes
    bool changed = true;

    while (changed) {
        changed = false;
        for (int i = 0; i < basicBlocks.size(); i++) {     //still start from 0, process all basic blocks
            std::set<std::pair<std::string, std::string>> new_availCopies = basicBlockComputeAvailableCopySets(basicBlocks[i]);
            if (new_availCopies != availCopies[i]) {      //check if the reaches set has changed, if so, update the reaches set and set changed to true
                availCopies[i] = new_availCopies;
                changed = true;
            }
        }
    }


}

std::set<std::pair<std::string, std::string>> ACS::basicBlockComputeAvailableCopySets(BasicBlock* basicBlock) {
    //compute the available copy statements for the basic block
    //availCopies[i] = (∩ availCopies[p]) - killed(i) U gen(i), for all predecessors p of i
    //the availCopies[] stored, is actually the out-availCopies

    //initialise the availCopies set for this basic block to the universe of all copy statements
    //but if there are no predecessors, then the availCopies set is empty
    std::set<std::pair<std::string, std::string>> availCopiesSet = allCopyStatements;

    //in-availCopies for the first instruction is the intersection of the out-availCopies of all the predecessors
    std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
    if (predecessors.empty()) {
        availCopiesSet = {};
    } else {
        for (BasicBlock* predecessor : predecessors) {
            //get the availCopies set for the predecessor
        int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
        std::set<std::pair<std::string, std::string>> predecessorAvailCopies = availCopies[predecessorIndex];

        //intersect the predecessor availCopies set with the availCopies set
        std::set<std::pair<std::string, std::string>> intersectedAvailCopies;
        std::set_intersection(availCopiesSet.begin(), availCopiesSet.end(), predecessorAvailCopies.begin(), predecessorAvailCopies.end(), std::inserter(intersectedAvailCopies, intersectedAvailCopies.begin()));
        availCopiesSet = intersectedAvailCopies;
       
        }
    }

    //then we process all the instructions in the basic block linearly downwards
    //to compute the out-reaches for each instruction
    const std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end(); ++it) {
        
        BaseNode* instruction = *it;
        std::set<std::pair<std::string, std::string>> generatedCopyStatements = AnalysisTools::getGeneratedCopyStatementsAtNode(instruction);
        std::set<std::pair<std::string, std::string>> killedCopyStatements = AnalysisTools::getKilledCopyStatementsAtNode(instruction, allCopyStatements);

        //remove the killed copy statements from the avail set
        for (std::pair<std::string, std::string> copyStatement : killedCopyStatements) {
            availCopiesSet.erase(copyStatement);
        }

        //add the generated expressions to the avail set
        for (std::pair<std::string, std::string> copyStatement : generatedCopyStatements) {
            availCopiesSet.insert(copyStatement);
        }
    }
    return availCopiesSet;
}


