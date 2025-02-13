#pragma once

#include <string>
#include <memory>
#include "BasicBlock.h"
#include "AnalysisTools.h"
#include "AVAIL.h"
#include "ExpressionNode.h"
#include <map>
#include "BaseNode.h"
#include "InsertableBasicBlock.h"

class PREOptimizer {

    public:

        PREOptimizer(std::shared_ptr<EntryNode> entryNode, std::unordered_map<std::string, std::string> irDatatypeMap, int nextProgramTempVariableCount);

        //runs PRE (once - same effect as running iteratively) until no more partial redundancies are found
        //returns true if any changes were made
        bool runPartialRedundancyElimination();

        //runs PRE + CP repeatedly until no more partial redundancies are found
        bool iteratePRE_CopyPropagation();

        int getNextProgramTempVariableCount() const;

        //returns the irDatatypeMap (may be updated PRE)
        std::unordered_map<std::string, std::string> getUpdatedIRDatatypeMap() const;

    private:

        std::shared_ptr<EntryNode> entryNode;
        std::shared_ptr<BasicBlock> startBasicBlock = nullptr;
        std::vector<std::shared_ptr<BasicBlock>> basicBlocks;
        std::set<std::string> allExpressions;    //need for complementation for the second check
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> nodeLatestExpressionsSets;
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> nodeOutUsedSets;
        int nextProgramTempVariableCount;
        std::unordered_map<std::string, std::string> irDatatypeMap;    //might need to add new variables
        std::unordered_map<std::string, std::string> irExpressionToDatatypeMap;   //needed to get the datatype of an expression, to create new temporaries

        //each expression has a new temporary variable _s associated with it
        //for expressions that need to be PRE optimized, it stores
        //expression: temp var       (e.g. x+y: _s1)  
        std::unordered_map<std::string, std::string> expressionToTempVarMap;
        
        //for all expressions, it stores
        //expression: first node of the expression       (e.g. x+y: Node(z=x+y))
        //so like [expression: clone-able node]      - because for each instance you replace, you actually want to create a new node, to hold the invariant that each node pointer is unique
        //(stores an expression node, so that it can be cloned - after this you go to expressionToTempVarMap to replace the dest variable)
        std::unordered_map<std::string, std::shared_ptr<ExpressionNode>> allExpressionsToCloneableNodesMap;

        //adds new basic blocks to the flowgraph to basic blocks that have more than one predecessor
        //adds the new basic block between that node and its predecessor
        void addNewBasicBlocksToMultiplePredecessorNodes();   

        //for each instruction in basic block, does 2 checks and adds/modifies instructions
        bool basicBlockRemovePartialRedundancy(const std::shared_ptr<BasicBlock>& basicBlock);

        // either fetches the temp variable from the expressionToTempVarMap, or creates a new temp variable and adds it to the expressionToTempVarMap
        // adds a declaration statement to the start of the program
        std::string getOrAddNewTempVariableForExpression(std::string &expression);

        ///CHECKS: returns set of expressions that are in (latest[B] ∩ out-used[B])
        //e.g. at this node, need to add t=x+y, so return x+y
        //returns the expressions to add to a temp assignment at that instruction
        std::set<std::string> getTempExpressionsToAdd(std::shared_ptr<BaseNode> instruction);

        //gets the temp assignment node for the expression, to be added to the instruction
        //either retrieves it from expressionToTempVarMap (and allExpressionsToCloneableNodesMap)
        //or creates a new temp variable and adds it to the expressionToTempVarMap (note the allExpressionsToCloneableNodesMap contains all the expressions anyways)
        std::shared_ptr<ExpressionNode> getNewTempExpressionNodeToAdd(std::string &expression);

        ///CHECKS: does the check for (¬latest[B] ∪ out-used[B])
        //you should pass in an expression already in e_useB, to check if it should be replaced by a temp variable in the expressionToTempVarMap
        bool isExpressionReplacedByTemp(std::shared_ptr<BaseNode> instruction, std::string &expression);


};