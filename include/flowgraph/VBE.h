#pragma once

#include "BaseDataFlowAnalysis.h"

//Very busy expressions (or Anticipated expressions) analysis
//like backwards version of AVAIL (intersection of all successors) 

class VBE : public BaseDataFlowAnalysis<std::set<std::string>> {

    public:

        VBE(BasicBlock* entryBasicBlock);

        void printBlockDataFlowSets() override;

    private:

        std::set<std::string> allExpressions;            //universe of all expressions in the program

        
    protected:
        
        std::set<std::string> meetOperation(const std::set<std::string>& current_vbe_set, const std::set<std::string>& successor_vbe_set) override;

        std::set<std::string> transferFunction(std::shared_ptr<BaseNode> instruction, const std::set<std::string>& out_vbe_set) override;



};