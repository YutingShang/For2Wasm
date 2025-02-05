#pragma once

#include "BaseDataFlowAnalysis.h"

//available expression analysis for partial redundancy elimination
///NOTE: this is slightly different to the AVAIL analysis, since expressions are now available if 
//1. they are available OR anticipated (could be made available by the instruction)
//2. and it is not killed by the instruction

class AVAIL_PRE : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        //constructor for AVAIL_PRE - initialises the base class with the entry basic block, FORWARD analysis direction, and the allExpressions set
        //will call the computeDataFlowSets() method in the base class to compute the dataflow sets for each basic block and instruction node
        AVAIL_PRE(std::shared_ptr<BasicBlock> entryBasicBlock);

        void printBlockDataFlowSets() override;

    private:

        //member variables
        std::set<std::string> allExpressions;            //universe of all expressions in the program
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesAnticipatedInExpressions;  //anticipated[B].in - used for earliest - to calculate latest

        
    protected:
        
        //meet operation for AVAIL
        std::set<std::string> meetOperation(const std::set<std::string>& current_pre_avail_set, const std::set<std::string>& predecessor_pre_avail_set) override;

        //transfer function for AVAIL
        std::set<std::string> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& in_pre_avail_set) override;

};