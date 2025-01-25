#pragma once

#include "BaseDataFlowAnalysis.h"

//class for postponable expressions analysis - forwards analysis, taking intersection of predecessors

class POST : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        POST(BasicBlock* entryBasicBlock);

        void printBlockDataFlowSets() override;

    private:

        std::set<std::string> allExpressions;
        std::unordered_map<BaseNode*, std::set<std::string>> allNodesEarliestExpressions;

    protected:

        std::set<std::string> meetOperation(const std::set<std::string>& current_post_set, const std::set<std::string>& predecessor_post_set) override;

        std::set<std::string> transferFunction(BaseNode* instruction, const std::set<std::string>& in_post_set) override;

};