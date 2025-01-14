#pragma once

#include "BasicBlock.h"
#include <set>
#include <unordered_map>
class PropagationOptimizer
{

public:
    PropagationOptimizer(BasicBlock *entryBasicBlock);

    void runCopyPropagation();

    void runConstantPropagation();

    enum PropagationType
    {
        COPY_PROPAGATION,
        CONSTANT_PROPAGATION
    };

private:
    BasicBlock *entryBasicBlock;
    std::vector<BasicBlock *> basicBlocks;
    std::set<std::pair<std::string, std::string>> allCopyStatements; // all copy statements in the program
    // NOTE: using a set to store the ACTUAL STRING representation of the reaching definitions
    //  since we will remove duplicates from the reaching definition points set (e.g. y = 8 and y = 8 are semantically the same definition)
    std::vector<std::set<std::pair<std::string, std::string>>> availCopies;

    bool basicBlockCopyPropagation(BasicBlock *basicBlock, PropagationType propagationType);

    // checks if the variable is an internal temporary variable
    bool isInternalTemporaryVariable(std::string var);

    // checks if the variable is a constant
    bool isConstant(std::string var);

    // iteratively find the final (non-temporary) replacement variable for a given variable
    //  e.g. if (x, y) and (y, z), then return z
    std::string getFinalReplacementVariable(std::string var, std::set<std::pair<std::string, std::string>> &inAvailCopiesSet);

    void runPropagation(PropagationType propagationType);
};