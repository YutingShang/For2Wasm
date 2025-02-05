#pragma once

#include "BaseDataFlowAnalysis.h"

// this is the analysis for "Available Copy Statements" - forward analysis with intersection meet operation
// used for copy and constant propagation
// similar to the AVAIL analysis, but for copy statements instead of expressions


//each availCopies set is a set of tuples of the form (var, replacementVar), for the available copy statements

class ACS : public BaseDataFlowAnalysis<std::set<std::pair<std::string, std::string>>> {

    public:

        ACS(std::shared_ptr<BasicBlock> entryBasicBlock);

        void printBlockDataFlowSets() override;

    private:

        //set of all copy statements (from MovNodes) in the program
        std::set<std::pair<std::string, std::string>> allCopyStatements;

    protected:

        //meet operation for ACS
        std::set<std::pair<std::string, std::string>> meetOperation(const std::set<std::pair<std::string, std::string>>& current_availCopies_set, const std::set<std::pair<std::string, std::string>>& predecessor_availCopies_set) override;

        //transfer function for ACS
        std::set<std::pair<std::string, std::string>> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::pair<std::string, std::string>>& in_availCopies_set) override;

};