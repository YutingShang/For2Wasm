#pragma once

#include "BaseDataFlowAnalysis.h"

//instance class for live variable analysis for a flowgraph
//inherits from BaseDataFlowAnalysis template class
//the template parameter is the type of the data flow set (in this case, a set of strings)

class LVA : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        //constructor for LVA - initialises the base class with the entry basic block, BACKWARD analysis direction, and an empty set
        //will call the computeDataFlowSets() method in the base class to compute the dataflow sets for each basic block and instruction node
        LVA(BasicBlock* entryBasicBlock);

        void printBlockDataFlowSets() override;

    protected:

        //meet operation for LVA
        std::set<std::string> meetOperation(const std::set<std::string>& current_live_set, const std::set<std::string>& successor_live_set) override;

        //transfer function for LVA
        std::set<std::string> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& out_live_set) override;

};