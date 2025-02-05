#pragma once

#include <set>
#include <memory>
#include "BaseDataFlowAnalysis.h"

//instance class for available expression analysis (need to keep track of basic blocks state for a flowgraph)
//inherits from BaseDataFlowAnalysis template class
//the template parameter is the type of the data flow set (in this case, a set of expressions strings)

class AVAIL : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        //constructor for AVAIL - initialises the base class with the entry basic block, FORWARD analysis direction, and the allExpressions set
        //will call the computeDataFlowSets() method in the base class to compute the dataflow sets for each basic block and instruction node
        AVAIL(std::shared_ptr<BasicBlock> entryBasicBlock);

        void printBlockDataFlowSets() override;

    private:

        //member variables
        std::set<std::string> allExpressions;            //universe of all expressions in the program

        
    protected:
        
        //meet operation for AVAIL
        std::set<std::string> meetOperation(const std::set<std::string>& current_avail_set, const std::set<std::string>& predecessor_avail_set) override;

        //transfer function for AVAIL
        std::set<std::string> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& in_avail_set) override;

};