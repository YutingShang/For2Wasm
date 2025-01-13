#pragma once

#include "BasicBlock.h"
#include <set>
#include <unordered_map>



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
        static std::set<std::string> getKilledExpressionsAtNode(BaseNode* node, std::set<std::string> &allExpressions);

        //returns the set of definitions in a program
        //definitions in a program is a dictionary of {var: [list of definition points (i.e. nodes)]}, since a variable can have multiple definitions at different nodes
        static std::unordered_map<std::string, std::set<BaseNode*>> getAllProgramDefinitionPoints(BaseNode* entryNode);

        // returns the set of ACTUAL STRING definitions at a node
        // either the generated expressions for an ExpressionNode, or the src for a MovNode
        static std::set<std::string> getDefinitionsAtNode(BaseNode* node);


};