#pragma once

#include "AnalysisTools.h"

//class for reaching definitions analysis

class RD {

    public:

        RD(BasicBlock* entryBasicBlock);

        //get the reaching definition points for each basic block
        std::vector<std::unordered_map<std::string, std::set<BaseNode*>>> getReachingDefPoints();

        std::vector<BasicBlock*> getBasicBlocksUsed();

        //all definition points for the entire program
        // std::unordered_map<std::string, std::set<BaseNode*>> getAllDefinitionPoints();

    private:

        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;
        // std::unordered_map<std::string, std::set<BaseNode*>> allDefinitionPoints;
        //vector for reaching definitions for each basic block
        //each vector element for a RD 'set' is actually a map of {var: [list of definition points (i.e. nodes) for that var]}
        std::vector<std::unordered_map<std::string, std::set<BaseNode*>>> reachingDefPoints;   

        //computes the reaching definitions for each basic block
        void computeReachingDefinitionPoints();

        //computes the reaching definitions for a single basic block
        std::unordered_map<std::string, std::set<BaseNode*>> basicBlockComputeReachingDefinitionPoints(BasicBlock* basicBlock);

        void printReachingDefinitionPoints();
};