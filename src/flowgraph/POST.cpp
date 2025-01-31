#include "POST.h"
#include "VBE.h"
#include "AVAIL.h"


POST::POST(BasicBlock* entryBasicBlock) : BaseDataFlowAnalysis<std::set<std::string>>(entryBasicBlock, AnalysisDirection::FORWARD) {
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    allExpressions = AnalysisTools::getAllProgramExpressions(rootNode);

    //get all earliest[B] expressions in the program
    allNodesEarliestExpressions = AnalysisTools::getAllNodesEarliestExpressions(entryBasicBlock);


    //set the initial set to the universe of all expressions
    setInitialSet(allExpressions);

    //compute the dataflow sets for each basic block and instruction node
    computeDataFlowSets();
}

void POST::printBlockDataFlowSets() {
    for (int i = 0; i < basicBlocks.size(); i++)
    {
        std::set<std::string> basicBlockPostSet = blockDataFlowSets[i];
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        std::cout << "Post Expressions Out-Sets: \n {";
        
        for (const auto &postExpr : basicBlockPostSet)
        { // get the dataflow set for the basic block
            std::cout << postExpr << ", ";
        }
        std::cout << "}\n-------------------------\n"
                  << std::endl;
    }
}


//meet operation for POST - returns the intersection of two sets
std::set<std::string> POST::meetOperation(const std::set<std::string>& current_post_set, const std::set<std::string>& predecessor_post_set) {
    std::set<std::string> new_post_set;
    //take intersection of the two sets by inserting all elements from both sets into the new set
    std::set_intersection(current_post_set.begin(), current_post_set.end(), predecessor_post_set.begin(), predecessor_post_set.end(), std::inserter(new_post_set, new_post_set.begin()));
    return new_post_set;
}

//transfer function for AVAIL_PRE - returns the set of variables that are available at the after processing the current instruction (going downwards)
std::set<std::string> POST::transferFunction(BaseNode* instruction, const std::set<std::string>& in_post_set) {
    std::set<std::string> out_post_set = in_post_set;

    //get the earliest[B] and used/referenced expressions from the instruction
    std::set<std::string> earliestExpressions = allNodesEarliestExpressions[instruction];
    std::set<std::string> usedExpressions = instruction->getReferencedExpressions();

    //add the earliest[B] expressions to the post set
    out_post_set.insert(earliestExpressions.begin(), earliestExpressions.end());

    //remove the used expressions from the post set
    for (std::string expr : usedExpressions) {
        out_post_set.erase(expr);
    }

    return out_post_set;
}

std::unordered_map<BaseNode*, std::set<std::string>> POST::getNodesEarliestExpressionsSets() {
    return allNodesEarliestExpressions;
}

