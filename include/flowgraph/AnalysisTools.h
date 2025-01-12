#pragma once

#include "BasicBlock.h"


// a static class for common functionality for dataflow analyses
class AnalysisTools {

    public:

        // constructor 
        AnalysisTools() = delete;

        //returns a vector of all the basic blocks in the flowgraph
        //uses BFS to get the basic blocks roughly in flowgraph order
        static std::vector<BasicBlock*> getBasicBlocks(BasicBlock* entryBasicBlock);

        //returns the universe of all expressions in the program
        static std::set<std::string> getAllProgramExpressions(BaseNode* entryNode);

        //returns the set of expressions Ex that are killed by the node (specific to a program)
        static std::set<std::string> getKilledProgramExpressions(BaseNode* node, std::set<std::string> &allExpressions);


};