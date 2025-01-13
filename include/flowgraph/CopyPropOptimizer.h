#pragma once

#include "BasicBlock.h"
#include <set>
#include <unordered_map>
class CopyPropOptimizer {

    public:

        CopyPropOptimizer(BasicBlock* entryBasicBlock);

        void runCopyPropagation();

    private:

        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;
        std::vector<std::set<std::string>> reachingDefinitions;

        //a map to store the replacements for each variable
        std::unordered_map<std::string, std::string> replacementMap;

        bool basicBlockCopyPropagation(BasicBlock* basicBlock);


};