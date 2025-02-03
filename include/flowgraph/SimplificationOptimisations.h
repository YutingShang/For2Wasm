#pragma once

#include "BaseNode.h"
#include <stack>
#include "IfNode.h"
#include "IfElseNode.h"
#include "LoopNode.h"
#include "LoopCondNode.h"

//static class provides tools such as `If simplification` - removing them where possible, and removes empty control flow constructs

class SimplificationOptimisations {

    public:

        SimplificationOptimisations() = delete;   //static class with methods only

        //repeatedly removes empty control flow constructs until no more can be removed
        static void removeAllEmptyControlFlowConstructs(std::shared_ptr<BaseNode> root);

        //only does one pass through the IR tree
        static bool removeEmptyStatements(std::shared_ptr<BaseNode> root);

};