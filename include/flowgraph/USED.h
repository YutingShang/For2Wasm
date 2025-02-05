#pragma once

#include <set>
#include <memory>
#include "BaseDataFlowAnalysis.h"

//used expressions analysis - backwards union, similar to LVA but for expressions instead of variables
//used for partial redundancy elimination

class USED : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        USED(std::shared_ptr<BasicBlock> entryBasicBlock);

        void printBlockDataFlowSets() override;

        //returns the latest expressions set for each node (which is calculated in order to compute the used set)
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> getNodesLatestExpressionsSets();

    private:

        // std::set<std::string> allExpressions;            //universe of all expressions in the program - used for complementation
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesLatestExpressions;  //latest[B] for all nodes
        

    protected:

        std::set<std::string> meetOperation(const std::set<std::string>& current_used_set, const std::set<std::string>& successor_used_set) override;

        std::set<std::string> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& out_used_set) override;

};