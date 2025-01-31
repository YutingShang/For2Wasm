#pragma once

#include "BasicBlock.h"
#include "AnalysisTools.h"
#include "AVAIL.h"
#include "ExpressionNode.h"
class PREOptimizer {

    public:

        PREOptimizer(BasicBlock* entryBasicBlock, int nextProgramTempVariableCount);

        void iteratePartialRedundancyElimination();

        bool partialRedundancyEliminationOnce();

        int getNextProgramTempVariableCount();

    private:

        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;
        std::set<std::string> allExpressions;    //need for complementation for the second check
        std::unordered_map<BaseNode*, std::set<std::string>> nodeLatestExpressionsSets;
        std::unordered_map<BaseNode*, std::set<std::string>> nodeOutUsedSets;
        int nextProgramTempVariableCount;

        //each expression has a new temporary variable _s associated with it
        //store the mapping so we can process the flowgraph ONCE, and process all expressions at each node
        ///QUESTION: how to handle between iterations?
        ///QUESTION: maybe I need to map the actual node??? Or like create a new node for the temp assignment, or change the node
        ///LIKE I could keep track of the original node, then maybe copy it over yeah... Add a copy constructor to each node 

        //for expressions that need to be PRE optimized, it stores
        //expression: temp var       (e.g. x+y: _s1)  
        std::unordered_map<std::string, std::string> expressionToTempVarMap;
        
        //for all expressions, it stores
        //expression: first node of the expression       (e.g. x+y: Node(z=x+y))
        //so like [expression: clone-able node]      - because for each instance you replace, you actually want to create a new node, to hold the invariant that each node pointer is unique
        //(stores an expression node, so that it can be cloned - after this you go to expressionToTempVarMap to replace the dest variable)
        std::unordered_map<std::string, ExpressionNode*> allExpressionsToCloneableNodesMap;

        //adds new basic blocks to the flowgraph to basic blocks that have more than one predecessor
        //adds the new basic block between that node and its predecessor
        void addNewBasicBlocksToMultiplePredecessorNodes();   

        //for each instruction in basic block, does 2 checks and adds/modifies instructions
        bool basicBlockRemovePartialRedundancy(BasicBlock* basicBlock);

        // also adds a declaration statement to the start of the program
        // and adds it to the expressionToTempVarMap so it can be used in other nodes
        std::string getOrAddNewTempVariableForExpression(std::string &expression);

        ///CHECKS: returns set of expressions that are in (latest[B] ∩ out-used[B])
        //e.g. at this node, need to add t=x+y, so return x+y
        //returns the expressions to add to a temp assignment at that instruction
        std::set<std::string> getTempExpressionsToAdd(BaseNode* instruction);

        //gets the temp assignment node for the expression, to be added to the instruction
        //either retrieves it from expressionToTempVarMap (and allExpressionsToCloneableNodesMap)
        //or creates a new temp variable and adds it to the expressionToTempVarMap (note the allExpressionsToCloneableNodesMap contains all the expressions anyways)
        ExpressionNode* getNewTempExpressionNodeToAdd(std::string &expression);

        ///CHECKS: does the check for (¬latest[B] ∪ out-used[B])
        //you should pass in an expression already in e_useB, to check if it should be replaced by a temp variable in the expressionToTempVarMap
        bool isExpressionReplacedByTemp(BaseNode* instruction, std::string &expression);

      




};