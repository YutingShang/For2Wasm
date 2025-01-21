#pragma once

#include "BasicBlock.h"
#include <set>
#include <unordered_map>
class PropagationOptimizer
{

public:
    PropagationOptimizer(BasicBlock *entryBasicBlock);

    //returns whether the copy propagation changed the program
    bool runCopyPropagation();

    //returns whether the constant propagation changed the program
    bool runConstantPropagation();

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

    bool basicBlockPropagation(BasicBlock *basicBlock, PropagationType propagationType);

    // checks if the variable is an internal temporary variable
    bool isInternalTemporaryVariable(std::string var);

    // checks if the variable is a constant
    bool isConstant(std::string var);

    // iteratively find the final (non-constant, non-temp) replacement variable for a given variable
    //  e.g. if (x, y) and (y, z), then return z
    ///WARNING: this final replacement variable could be a temporary variable if the input variable is a TEMP!, so we need to check if it is a temporary variable after the function call
    std::string getFinalReplacementVariable(std::string var, std::set<std::pair<std::string, std::string>> &inAvailCopiesSet);

    //remove the (MOV _t0 a) instruction, if you are propagating _t0 -> a
    void removeMovTempInstruction(BasicBlock *currentBasicBlock, BaseNode *currentInstructionNode, std::string tempVar);

    //start backwards from this node, remove the (MOV _t0 a) instruction, if you are propagating _t0 -> a
    //returns true if the instruction was found and removed, false otherwise
    bool basicBlockRemoveMovTempInstruction(BasicBlock *basicBlock, std::string tempVar, BaseNode* beginBackwardsFromThisNode = nullptr);

    //returns whether the propagation changed the program
    bool runPropagation(PropagationType propagationType);
};