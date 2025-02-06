#include "DeadCodeElimination.h"
#include "LVA.h"
#include "EntryNode.h"

DeadCodeElimination::DeadCodeElimination(std::shared_ptr<EntryNode> entryNode) : entryNode(entryNode) {
    //draw the flowgraph at the start of each iteration
}

void DeadCodeElimination::iterateDeadCodeElimination()
{
    bool runDCE = true;
    while (runDCE)
    {
        runDCE = deadCodeEliminationOnce();
    }
}

bool DeadCodeElimination::deadCodeEliminationOnce()
{
    //draw/redraw the flowgraph from the entry node and initialise the basic blocks and live sets
    startBasicBlock = AnalysisTools::drawFlowgraph(entryNode);

    // Analysis: LVA
    LVA lva(startBasicBlock);
    std::vector<std::shared_ptr<BasicBlock>> basicBlocks = lva.getBasicBlocksUsed();
    nodeLiveSets = lva.getNodeOutDataFlowSets();

    //order that the nodes will be visited for transformation
    std::vector<std::shared_ptr<BaseNode>> originalTreeNodes = AnalysisTools::getIRNodesInFlowgraphOrder(basicBlocks);

    // Transformation: Remove dead code
    bool removed = false;
    for (std::shared_ptr<BaseNode> node : originalTreeNodes) {
        removed |= removeDeadCodeForNode(node);
    }

    return removed;
}

bool DeadCodeElimination::removeDeadCodeForNode(std::shared_ptr<BaseNode> instruction) {
    std::set<std::string> out_live_set = nodeLiveSets[instruction];
    std::set<std::string> def_set = instruction->getDefinedVariables();

    if (!def_set.empty())
    { // if the instruction does assign/define a variable

        // check if the variable is NOT in the (out)live set
        /// NOTE: assumes only one variable defined per instruction
        assert(def_set.size() == 1);
        std::string var = *def_set.begin(); 

        if (out_live_set.find(var) == out_live_set.end())
        { // variable is not live after this instruction
            // this instruction is dead code and we can remove it - from the instruction list and also the ir tree
            instruction->removeCurrentNodeFromIRTree();
            return true;    //modified
        }
    }

    return false;  //not modified
}