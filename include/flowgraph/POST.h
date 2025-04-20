#pragma once

#include "BaseDataFlowAnalysis.h"

//class for postponable expressions analysis - forwards analysis, taking intersection of predecessors

class POST : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        POST(std::shared_ptr<BasicBlock> entryBasicBlock);
        //overload
        POST(std::shared_ptr<BasicBlock> entryBasicBlock, std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>>& allNodesEarliestExpressions);

        void printBlockDataFlowSets() override;

        //returns the earliest expressions set for each node (calculated from AnalysisTools::getAllNodesEarliestExpressions)
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> getNodesEarliestExpressionsSets();

    private:

        std::set<std::string> allExpressions;
        std::map<std::weak_ptr<BaseNode>, std::set<std::string>, std::owner_less<std::weak_ptr<BaseNode>>> allNodesEarliestExpressions;

    protected:

        std::set<std::string> meetOperation(const std::set<std::string>& current_post_set, const std::set<std::string>& predecessor_post_set) override;

        std::set<std::string> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& in_post_set) override;

};