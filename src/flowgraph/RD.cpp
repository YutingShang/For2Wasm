#include "RD.h"
#include "ExpressionNode.h"
#include "MovNode.h"
#include <iostream>

RD::RD(BasicBlock* entryBasicBlock): entryBasicBlock(entryBasicBlock) {
    //a forward analysis
    basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);
    BaseNode* entryNode = entryBasicBlock->get_instructions_copy().front();
    allDefinitionPoints = AnalysisTools::getAllProgramDefinitionPoints(entryNode);
    computeReachingDefinitions();    //will initialise the reachingDefs vector
 }

std::vector<std::unordered_map<std::string, std::set<BaseNode*>>> RD::getReachingDefs() {
    for (int i = 0; i < basicBlocks.size(); i++) {
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Reaching definitions: " << std::endl;
        for (const auto& [var, definitionPoints] : reachingDefs[i]) {
            std::cout << "Variable: " << var << ", Definitions: ";
            for (BaseNode* defNode : definitionPoints) {
                std::set<std::string> definitions = AnalysisTools::getDefinitionsAtNode(defNode);
                //should only be one definition per node
                for (const std::string& def : definitions) {
                    std::cout << def << " ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << "-------------------------\n" << std::endl;
    }
    return reachingDefs;
}

std::vector<BasicBlock*> RD::getBasicBlocksUsed() {
    return basicBlocks;
}

std::unordered_map<std::string, std::set<BaseNode*>> RD::getAllDefinitionPoints() {
    return allDefinitionPoints;
}

void RD::computeReachingDefinitions() {

    //Reaching definitions, forward analysis taking union of predecessors   

    //initialise the reachingDefs vector, to empty sets for each basic block
    reachingDefs = std::vector<std::unordered_map<std::string, std::set<BaseNode*>>>(basicBlocks.size());

    //iterate until no more changes
    bool changed = true;

    while (changed) {
        changed = false;
        for (int i = 0; i < basicBlocks.size(); i++) {     //still start from 0, process all basic blocks
            std::unordered_map<std::string, std::set<BaseNode*>> new_reaches_set = basicBlockComputeReachingDefinitions(basicBlocks[i]);
            if (new_reaches_set != reachingDefs[i]) {      //check if the reaches set has changed, if so, update the reaches set and set changed to true
                reachingDefs[i] = new_reaches_set;
                changed = true;
            }
        }
    }


}

std::unordered_map<std::string, std::set<BaseNode*>> RD::basicBlockComputeReachingDefinitions(BasicBlock* basicBlock) {
    //compute the reaching definitions for the basic block
    //RD[i] = (U RD[p]) - killed(i) U def(i), for all predecessors p of i
    //the RD[] stored, is actually the out-reaches

    //the reaches set for this basic block
    std::unordered_map<std::string, std::set<BaseNode*>> reachesSet = {};

    //in-reaches for the first instruction is the union of the out-reaches of all the predecessors
    std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
    for (BasicBlock* predecessor : predecessors) {
        //get the reaches set for the predecessor
        int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
        std::unordered_map<std::string, std::set<BaseNode*>> predecessorReachesSet = reachingDefs[predecessorIndex];

        //union the predecessor reaches set with the reaches set
        // combine two maps like this: {var: [list of definitions for that var]}
        for (const auto& [var, definitions] : predecessorReachesSet) {
            reachesSet[var].insert(definitions.begin(), definitions.end());
        }
       
    }

    //then we process all the instructions in the basic block linearly downwards
    //to compute the out-reaches for each instruction
    const std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end(); ++it) {
        
        BaseNode* instruction = *it;
        std::set<std::string> def_set = instruction->getDefinedVariables();
        //assume one definition per instruction max
        if (!def_set.empty()) {
            assert(def_set.size() == 1);
            std::string definedVariable = *def_set.begin();
            // std::set<std::string> definedExpressions = AnalysisTools::getDefinitionsAtNode(instruction);
            std::set<BaseNode*> newDefinitionPoint = {instruction};

        //replace (or add) the def_var: [definedExpressions] in the reaches set
        //this does both "- killed(i) U def(i)" in one step
        reachesSet[definedVariable] = newDefinitionPoint;
        }
    }
    return reachesSet;
}


