#pragma once

#include "AnalysisTools.h"

//class for reaching definitions analysis

class RD {

    public:

        RD(BasicBlock* entryBasicBlock);

        std::vector<std::unordered_map<std::string, std::set<std::string>>> getReachingDefs();

        std::vector<BasicBlock*> getBasicBlocks();

        std::unordered_map<std::string, std::set<std::string>> getAllDefinitions();

    private:

        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;
        std::unordered_map<std::string, std::set<std::string>> allDefinitions;
        //vector for reaching definitions for each basic block
        //each vector element for a RD 'set' is actually a map of {var: [list of definitions for that var]}
        std::vector<std::unordered_map<std::string, std::set<std::string>>> reachingDefs;   

        //computes the reaching definitions for each basic block
        void computeReachingDefinitions();

        //computes the reaching definitions for a single basic block
        std::unordered_map<std::string, std::set<std::string>> basicBlockComputeReachingDefinitions(BasicBlock* basicBlock);


};