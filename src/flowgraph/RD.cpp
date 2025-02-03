// #include "RD.h"
// #include "ExpressionNode.h"
// #include "MovNode.h"
// #include <iostream>

// RD::RD(BasicBlock* entryBasicBlock): entryBasicBlock(entryBasicBlock) {
//     //a forward analysis
//     basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);
//     std::shared_ptr<BaseNode> entryNode = entryBasicBlock->get_instructions_copy().front().lock();
//     // allDefinitionPoints = AnalysisTools::getAllProgramDefinitionPoints(entryNode);
//     computeReachingDefinitionPoints();    //will initialise the reachingDefPoints vector
//  }

// std::vector<std::unordered_map<std::string, std::set<std::weak_ptr<BaseNode>>>> RD::getReachingDefPoints() {
//     printReachingDefinitionPoints();
//     return reachingDefPoints;
// }

// void RD::printReachingDefinitionPoints() {
//         for (int i = 0; i < basicBlocks.size(); i++) {
//         std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
//         std::cout << "Reaching definition points: " << std::endl;
//         for (const auto& [var, definitionPoints] : reachingDefPoints[i]) {
//             std::cout << "Variable: " << var << ", Definitions: ";
//             for (std::weak_ptr<BaseNode> defNode : definitionPoints) {
//                 std::set<std::string> definitions = AnalysisTools::getDefinitionsAtNode(std::shared_ptr<BaseNode>(defNode));
//                 //should only be one definition per node
//                 for (const std::string& def : definitions) {
//                     std::cout << def << " ";
//                 }
//             }
//             std::cout << std::endl;
//         }
//         std::cout << "-------------------------\n" << std::endl;
//     }
// }

// std::vector<BasicBlock*> RD::getBasicBlocksUsed() {
//     return basicBlocks;
// }

// // std::unordered_map<std::string, std::set<BaseNode*>> RD::getAllDefinitionPoints() {
// //     return allDefinitionPoints;
// // }

// void RD::computeReachingDefinitionPoints() {

//     //Reaching definitions, forward analysis taking union of predecessors   

//     //initialise the reachingDefPoints vector, to empty sets for each basic block
//     reachingDefPoints = std::vector<std::unordered_map<std::string, std::set<std::weak_ptr<BaseNode>>>>(basicBlocks.size());

//     //iterate until no more changes
//     bool changed = true;

//     while (changed) {
//         changed = false;
//         for (int i = 0; i < basicBlocks.size(); i++) {     //still start from 0, process all basic blocks
//             std::unordered_map<std::string, std::set<std::weak_ptr<BaseNode>>> new_reaches_set = basicBlockComputeReachingDefinitionPoints(basicBlocks[i]);
//             if (new_reaches_set != reachingDefPoints[i]) {      //check if the reaches set has changed, if so, update the reaches set and set changed to true
//                 reachingDefPoints[i] = new_reaches_set;
//                 changed = true;
//             }
//         }
//     }


// }

// std::unordered_map<std::string, std::set<std::weak_ptr<BaseNode>>> RD::basicBlockComputeReachingDefinitionPoints(BasicBlock* basicBlock) {
//     //compute the reaching definitions for the basic block
//     //RD[i] = (U RD[p]) - killed(i) U def(i), for all predecessors p of i
//     //the RD[] stored, is actually the out-reaches

//     //the reaches set for this basic block
//     std::unordered_map<std::string, std::set<std::weak_ptr<BaseNode>>> reachesSet = {};

//     //in-reaches for the first instruction is the union of the out-reaches of all the predecessors
//     std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
//     for (BasicBlock* predecessor : predecessors) {
//         //get the reaches set for the predecessor
//         int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
//         std::unordered_map<std::string, std::set<std::weak_ptr<BaseNode>>> predecessorReachesSet = reachingDefPoints[predecessorIndex];

//         //union the predecessor reaches set with the reaches set
//         // combine two maps like this: {var: [list of definitions for that var]}
//         for (const auto& [var, definitions] : predecessorReachesSet) {
//             reachesSet[var].insert(definitions.begin(), definitions.end());
//         }
       
//     }

//     //then we process all the instructions in the basic block linearly downwards
//     //to compute the out-reaches for each instruction
//     const std::list<std::weak_ptr<BaseNode>>& instructions = basicBlock->get_instructions_reference();
//     for (auto it = instructions.begin(); it != instructions.end(); ++it) {
        
//         std::shared_ptr<BaseNode> instruction = it->lock();
//         std::set<std::string> def_set = instruction->getDefinedVariables();
//         //assume one definition per instruction max
//         if (!def_set.empty()) {
//             assert(def_set.size() == 1);
//             std::string definedVariable = *def_set.begin();
//             // std::set<std::string> definedExpressions = AnalysisTools::getDefinitionsAtNode(instruction);
//             std::set<std::weak_ptr<BaseNode>> newDefinitionPoint = {instruction};

//         //replace (or add) the def_var: [definedExpressions] in the reaches set
//         //this does both "- killed(i) U def(i)" in one step
//         reachesSet[definedVariable] = newDefinitionPoint;
//         }
//     }
//     return reachesSet;
// }


