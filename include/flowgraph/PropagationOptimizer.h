#pragma once

#include "BasicBlock.h"
#include <set>
#include <map>

class PropagationOptimizer
{

public:
    PropagationOptimizer(std::shared_ptr<EntryNode> entryNode);

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
    std::shared_ptr<EntryNode> entryNode;
    std::shared_ptr<BasicBlock> startBasicBlock = nullptr;
    std::vector<std::shared_ptr<BasicBlock>> basicBlocks;
    //map from each node to the set of available copy statements for that node, the copy statements are of the form (var, replacementVar)
    std::map<std::weak_ptr<BaseNode>, std::set<std::pair<std::string, std::string>>, std::owner_less<std::weak_ptr<BaseNode>>> nodeAvailCopies;

    //returns whether the propagation changed the program
    bool runPropagation(PropagationType propagationType);

    //returns whether the propagation changed the program for a single basic block
    bool basicBlockPropagation(std::shared_ptr<BasicBlock> basicBlock, PropagationType propagationType);

    // checks if the variable is an internal temporary variable
    bool isInternalTemporaryVariable(std::string var);

    // checks if the variable is a constant
    bool isConstant(std::string var);

    // iteratively find the final (non-constant, non-temp) replacement variable for a given variable
    // e.g. if (x, y) and (y, z), then return z
    ///NOTE: this final replacement variable could be a temporary variable if the input variable is a TEMP!, so we need to check if it is a temporary variable after the function call
    std::string getFinalReplacementVariable(std::string var, std::set<std::pair<std::string, std::string>> &inAvailCopiesSet);

    //if you are replacing a temporary variable with a propagated value _t0 -> a
    //you need to start backwards from this node, remove the (MOV _t0 a) instruction, (otherwise temp variable left on the stack in WASM)
    //this is for the whole flowgraph, the next one is for a single basic block
    void removeMovTempInstruction(std::shared_ptr<BasicBlock> currentBasicBlock, std::shared_ptr<BaseNode> currentInstructionNode, std::string tempVar);

    //if you are replacing a temporary variable with a propagated value _t0 -> a
    //you need to start backwards from this node, remove the (MOV _t0 a) instruction, (otherwise temp variable left on the stack in WASM)
    //returns true if the instruction was found and removed, false otherwise
    bool basicBlockRemoveMovTempInstruction(std::shared_ptr<BasicBlock> basicBlock, std::string tempVar, std::shared_ptr<BaseNode> beginBackwardsFromThisNode = nullptr);
};