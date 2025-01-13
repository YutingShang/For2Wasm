#include "CopyPropOptimizer.h"
#include "RD.h"
#include <iostream>

CopyPropOptimizer::CopyPropOptimizer(BasicBlock* entryBasicBlock): entryBasicBlock(entryBasicBlock) { }

void CopyPropOptimizer::runCopyPropagation() {

    //get the reaching definitions for each basic block
    //Analysis: Reaching Definitions
    RD rd(entryBasicBlock);
    std::vector<std::unordered_map<std::string, std::set<BaseNode*>>> reachingDefs = rd.getReachingDefs();
    basicBlocks = rd.getBasicBlocksUsed();

}

bool CopyPropOptimizer::basicBlockCopyPropagation(BasicBlock* basicBlock) {
    return false;
}

