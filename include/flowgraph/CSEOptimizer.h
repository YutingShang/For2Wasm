#pragma once

#include <string>
#include <memory>
#include "BasicBlock.h"
#include "AnalysisTools.h"
#include "AVAIL.h"
#include <map>

//not a static class, unlike DeadCodeElimination
//needs to keep the temporary variable count as state for a particular program
class CSEOptimizer {

    public:

        //constructor - will set the member variables
        // should initialise the nextProgramTempVariableCount to 0 if its the first time CSE is run
        // the next time, feed the previous nextProgramTempVariableCount into the constructor
        CSEOptimizer(std::shared_ptr<EntryNode> entryNode, std::unordered_map<std::string, std::string> irDatatypeMap, int nextProgramTempVariableCount);

        //runs CSE (once - same effect as running iteratively) until no more common subexpressions are found
        bool runCommonSubexpressionElimination();

        //runs CSE + Copy Propagation repeatedly until no more common subexpressions are found
        bool iterateCSE_CopyPropagation();

        //returns the next program temp variable count
        int getNextProgramTempVariableCount() const;

        //returns the irDatatypeMap (may be updated CSE)
        std::unordered_map<std::string, std::string> getUpdatedIRDatatypeMap() const;

    private:
        
        //member variables
        std::shared_ptr<EntryNode> entryNode;
        std::shared_ptr<BasicBlock> startBasicBlock;            //start of the flowgraph - may be redrawn
        std::vector<std::shared_ptr<BasicBlock>> basicBlocks;            //vector of all basic blocks in the program
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> nodeAvailSets;      //vector of available expressions for each instruction node
        std::unordered_map<std::string, std::string> irDatatypeMap;

        //the next temporary variable count, initialised to 0
        //these are _s program temporary variables which are added to the resulting program(not to be confused with _t temp variables for internal use)
        int nextProgramTempVariableCount;

        //checks the current node to see if it contains any common expressions to be removed
        //returns true if the node has been modified, false otherwise
        bool removeCommonSubexpressionsForNode(std::shared_ptr<BaseNode> node);

        //helper function in CSE when going backwards on each control path
        //in each basic block, find the first occurrence of the expression
        //and replace it with a the temporary variable
        //beginBackwardsFromThisNode is the node to start searching backwards from (since for the current basic block, we start searching from the current instruction, not from the end of the basic block)
            //if it is nullptr, then default to starting from the end of the basic block
        bool basicBlockBackwardsFindAndReplaceExpression(
            std::shared_ptr<BasicBlock> basicBlock,
            std::string& expressionToFind,
            std::string& tempVar,
            std::shared_ptr<BaseNode> beginBackwardsFromThisNode = nullptr);

        //returns a new temporary variable name _s with the given IR datatype (also adds this to the irDatatypeMap)
        //also adds a declaration statement to the start of the program 
        std::string addNewProgramTempVariable(std::string IRdatatype);
  
    
};

