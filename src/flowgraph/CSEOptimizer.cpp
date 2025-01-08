#include "CSEOptimizer.h"
#include <iostream>

CSEOptimizer::CSEOptimizer(int nextTempVariableCount) : nextTempVariableCount(nextTempVariableCount) {}

void CSEOptimizer::iterateCommonSubexpressionElimination(BasicBlock *entryBasicBlock)
{
    //first get all the expressions in the program - needed to do CSE on the entire program
    BaseNode* rootNode = entryBasicBlock->get_instructions_copy().front();
    std::set<std::string> allExpressions = getAllProgramExpressions(rootNode);    

    bool runCSE = true;
    while (runCSE)
    {
        runCSE = commonSubexpressionEliminationOnce(entryBasicBlock, allExpressions);
        //if modified, rerun again to see any more common subexpressions
    }

}


bool CSEOptimizer::commonSubexpressionEliminationOnce(BasicBlock *entryBasicBlock, std::set<std::string> &allExpressions)
{
    // Analysis: AVAIL
    std::vector<BasicBlock *> basicBlocks = getBasicBlocks(entryBasicBlock);
    std::vector<std::set<std::string>> availSets = computeAvailSets(basicBlocks, allExpressions);

    // Transformation: Remove common subexpressions
    bool removed = false;
    for (BasicBlock *basicBlock : basicBlocks)
    {
        removed |= basicBlockRemoveCommonSubexpressions(basicBlock, basicBlocks, availSets, allExpressions);
    }

    return removed;
}

///NOTE: slightly modified from DeadCodeElimination.cpp - does not reverse the basic blocks
std::vector<BasicBlock *> CSEOptimizer::getBasicBlocks(BasicBlock *entryBasicBlock)
{
    // first get a list of all the basic blocks in the flowgraph
    // we want to propagate information down the flowgraph
    // since CSE is a forwards analysis
    // so we visit the basic blocks in forward order

    std::vector<BasicBlock *> basicBlocks;

    // get all the basic blocks in the flowgraph by BFS, and output into the basicBlocks vector
    std::queue<BasicBlock *> toExploreQueue;
    toExploreQueue.push(entryBasicBlock);
    std::set<BasicBlock *> seen;
    seen.insert(entryBasicBlock);

    while (!toExploreQueue.empty())
    {
        BasicBlock *current = toExploreQueue.front();
        basicBlocks.push_back(current); // visit the current basic block
        toExploreQueue.pop();

        // add all the successors to the queue if they are not already seen
        for (BasicBlock *successor : current->get_successors())
        {
            if (seen.find(successor) == seen.end())
            { // check 'seen' set
                toExploreQueue.push(successor);
                seen.insert(successor);
            }
        }
    }

    return basicBlocks;

}

std::set<std::string> CSEOptimizer::getAllProgramExpressions(BaseNode* root) {
    std::set<std::string> allExpressions;

    //IR tree traversal (not a graph)
    std::queue<BaseNode*> toExploreQueue;
    toExploreQueue.push(root);
    while (!toExploreQueue.empty()) {
        BaseNode* current = toExploreQueue.front();
        toExploreQueue.pop();
        //get generated expressions, insert into allExpressions set
        const std::set<std::string>& generatedExpressions = current->getGeneratedExpressions();
        allExpressions.insert(generatedExpressions.begin(), generatedExpressions.end());
        //add children to the queue
        for (BaseNode* child : current->getChildren()) {
            toExploreQueue.push(child);
        }
    }
    return allExpressions;
}

std::set<std::string> CSEOptimizer::getKilledProgramExpressions(BaseNode* node, std::set<std::string> &allExpressions) {
    std::set<std::string> killedExpressions;

    //get the defined variables of the node, e.g. {x}
    std::set<std::string> definedVariables = node->getDefinedVariables();

    //Then all E_x expressions (which contains the variable x) are killed by the node
    for (const auto &programExpr : allExpressions) {

        //split the expression based on symbols (+, -, *, /, |, &, !, <, > , <=, >=, ==)
        std::regex symbolRegex("([+\\-*/|&!<>=]+)");
        std::sregex_token_iterator exprIter(programExpr.begin(), programExpr.end(), symbolRegex, -1);     //split the expression based on symbols
        std::sregex_token_iterator exprEnd;      //default end iterator

        //check if the expression contains any of the defined variables
        bool containsDefinedVariable = false;
        while (exprIter != exprEnd && !containsDefinedVariable) {
            std::string exprToken = *exprIter;
            if (definedVariables.find(exprToken) != definedVariables.end()) {
                containsDefinedVariable = true;
            }
            ++exprIter;
        }
        if (containsDefinedVariable) {        //e.g. expression contains x, so it is killed by the node
            killedExpressions.insert(programExpr);
        }
    }
  
    return killedExpressions;
}

std::vector<std::set<std::string>> CSEOptimizer::computeAvailSets(std::vector<BasicBlock*> &basicBlocks, std::set<std::string> &allExpressions)
{
    /// NOTE: this is the AVAIL algorithm

    // have a vector of avail sets, one for each basic block
    // for entry (instruction!) node, avail set is initialised to empty (not the entire basic block)
        //NOTE: we will check for this in basicBlockComputeAvailSet
    // for all node, initialise to universe of all expressions
    std::vector<std::set<std::string>> availSets(basicBlocks.size(), allExpressions);

    //iterate until no more changes
    bool changed = true;

    while (changed) {
        changed = false;
        for (int i = 0; i < basicBlocks.size(); i++) {     //still start from 0, process all basic blocks
            std::set<std::string> new_avail_set = basicBlockComputeAvailSet(basicBlocks[i], basicBlocks, availSets, allExpressions);
            if (new_avail_set != availSets[i]) {      //check if the avail set has changed, if so, update the avail set and set changed to true
                availSets[i] = new_avail_set;
                changed = true;
            }
        }
    }
    // std::cout << "Avail sets: {" << std::endl;
    // for (const auto& availSet : availSets) {
    //     std::cout << "{";
    //     for (const auto& expr : availSet) {
    //         std::cout << expr << ", ";
    //     }
    //     std::cout << "}" << std::endl;
    // }
    // std::cout << "}" << std::endl;

    return availSets;
}

std::set<std::string> CSEOptimizer::basicBlockComputeAvailSet(BasicBlock *basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets, std::set<std::string> &allExpressions)
{
    //compute the avail set for the basic block
    ///NEW: avail[i] = (∩ avail[p]) - killed(i) U gen(i), for all predecessors p of i
    //the avail[] stored, is actually the out-avail

    std::set<std::string> availSet = allExpressions;     //initialise to universe of all expressions, to get intersection of all predecessors in a pairwise manner
   
    //in-avail for the first instruction is the intersection of the out-avail of all the predecessors
    //this intersection is empty if there are no predecessors
    std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
    if (predecessors.empty()) {
        availSet = {};
    } else {
        for (BasicBlock* predecessor : predecessors) {
   
            //get the avail[p] for the predecessor
            int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
            std::set<std::string> predecessorAvailSet = availSets[predecessorIndex];

            //intersect the predecessor avail set with the avail set, store in availSet
            std::set<std::string> intersectedAvailSet;
            std::set_intersection(availSet.begin(), availSet.end(), predecessorAvailSet.begin(), predecessorAvailSet.end(), std::inserter(intersectedAvailSet, intersectedAvailSet.begin()));
            availSet = intersectedAvailSet;
        }
    } 
    //availSet now temporarily stores the in-avail set of the basic block/first instruction (∩ avail[p])

    //then we process all the instructions in the basic block linearly downwards
    //so that availSet computes and stores the out-avail for each instruction node
    const std::list<BaseNode*> &instructions = basicBlock->get_instructions_reference();     //gets reference
    for (auto it = instructions.begin(); it != instructions.end(); ++it) {
        
        BaseNode* instruction = *it;
        std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();
        std::set<std::string> killedExpressions = getKilledProgramExpressions(instruction, allExpressions);

        //remove the killed expressions from the avail set
        for (std::string expr : killedExpressions) {
            availSet.erase(expr);
        }

        //add the generated expressions to the avail set
        for (std::string expr : generatedExpressions) {
            availSet.insert(expr);
        }
    }

    //the availSet now stores the out-avail set of the basic block/last instruction
    return availSet;
}

bool CSEOptimizer::basicBlockRemoveCommonSubexpressions(BasicBlock *basicBlock, std::vector<BasicBlock*> &basicBlocks, std::vector<std::set<std::string>> &availSets, std::set<std::string> &allExpressions)
{
    //iterate through the instructions in the basic block
    //if the current instruction generates an expression which is already available,
        // i.e. check the (in)avail set before the corresponding instruction
    // if the expression is in the (in)avail set, then deal with it in CSE
        // first replace that instruction with a MOV instruction with a temporary variable
        // on every control flow backwards, find the first occurrence of that expression (NOTE: maybe case where 2 backward paths merge)
        // and add a new MOV instruction to save the expression into the temporary variable
   
    //return true if the basic block has been modified, false otherwise
    bool modified = false;

    //in_avail set is the intersection of all the predecessors' (out)avail sets
    std::set<std::string> in_avail_set = allExpressions;
    //(most copy code from basicBlockComputeAvailSet)
    std::vector<BasicBlock*> predecessors = basicBlock->get_predecessors();
    if (predecessors.empty()) {
        in_avail_set = {};
    } else {
        for (BasicBlock* predecessor : predecessors) {
   
            //get the avail[p] for the predecessor
            int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
            std::set<std::string> predecessorAvailSet = availSets[predecessorIndex];

            //intersect the predecessor avail set with the avail set, store in availSet
            std::set<std::string> intersectedAvailSet;
            std::set_intersection(in_avail_set.begin(), in_avail_set.end(), predecessorAvailSet.begin(), predecessorAvailSet.end(), std::inserter(intersectedAvailSet, intersectedAvailSet.begin()));
            in_avail_set = intersectedAvailSet;
        }
    } 

    //now we have the in_avail set, for each instruction, check if the current instruction generates an expression which is already available
    //then we calculate the out_avail set for the instruction (which will be the in_avail set for the next instruction)

    std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
    for (auto it = instructions.begin(); it != instructions.end();) {
        //for each instruction, check if it generates an expression which is already available

        BaseNode* instruction = *it;
        std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();
        std::set<std::string> killedExpressions = getKilledProgramExpressions(instruction, allExpressions);

        if (generatedExpressions.empty()) {
            //the instruction does not generate any expressions
            ++it;
            continue;
        }
        //otherwise, there is at least one expression generated
        assert(generatedExpressions.size() == 1);     //should just be one expression that is generated, which we need to handle if it is already available
        std::string expr = *generatedExpressions.begin();

        if (in_avail_set.find(expr) != in_avail_set.end()) {    
            // std::cout <<"Current instruction: " << instruction->getText() << std::endl;
            // std::cout << "Expression " << expr << " is already available!" << std::endl;
            modified = true;
            //current instruction DOES generates an available expression!
            //handle with CSE transformation!
            
            //the instruction should be ArithmeticNode, LogicBinNode, LogicNotNode, or RelOpNode
            //these are all SimpleNode subclasses!!
            SimpleNode* simpleNode = dynamic_cast<SimpleNode*>(instruction);
            if (simpleNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not a SimpleNode subclass");
            }

            ///FIRST: replace the current instruction y:=e
            //with a MOV instruction with a new temporary variable y:= t
            std::string tempVar = getNewTempVariableName();
            assert(simpleNode->getDefinedVariables().size() == 1);
            std::string destVar = *simpleNode->getDefinedVariables().begin();  //the variable defined, i.e. y
            MovNode* movNode = new MovNode(destVar,tempVar);
            it = basicBlock->replace_instruction_node(it, movNode);    //updates the iterator to point to the next instruction
  
            ///SECOND: for every backwards control flow, find the first occurrence of the expression, and EDIT
            // do a DFS backwards from the current basic block as the start node
                //need function to go inside each basic block
            std::stack<BasicBlock *> toExploreStack;
            toExploreStack.push(basicBlock);
            std::set<BasicBlock *> seen;
            seen.insert(basicBlock);

            while (!toExploreStack.empty()) {
                BasicBlock *current = toExploreStack.top();
                toExploreStack.pop();
                //go inside each basic block
                
                bool expressionFound = false;
                if (current == basicBlock) {      //only for the first basic block, start backwards search from the current instruction
                    //beginBackwardsFromThisNode is the current instruction node (which was replaced with the movNode)
                    expressionFound = basicBlockBackwardsFindAndReplaceExpression(current, expr, tempVar, movNode);
                } else {             //for all other basic blocks, start backwards search from the end of the basic block
                    expressionFound = basicBlockBackwardsFindAndReplaceExpression(current, expr, tempVar);
                }
                
                if (!expressionFound){    //then we add the predecessors to the stack 
                    std::vector<BasicBlock*> predecessors = current->get_predecessors();
                    //check that there should be at least one predecessor, otherwise we have hit the entry instruction node on a backwards path
                    if (predecessors.empty()) {
                        throw std::runtime_error("ERROR while handling CSE: expression " + expr + " not found on a backwards path");
                    }
                    for (BasicBlock* predecessor : predecessors) {
                        if (seen.find(predecessor) == seen.end()) {     //if it is not already seen
                            toExploreStack.push(predecessor);
                            seen.insert(predecessor);
                        }
                    }
                }
                //otherwise we prune the DFS search up that backwards path (don't add the predecessors to the stack)
            }
            //iterator already points to the next instruction from replace_instruction_node
        }else{
            //the current instruction does not generate an available expression
            //so we just continue to the next instruction
            ++it;
        }

        //calculate the new (in)avail set for the next instruction node
        for (std::string expr : killedExpressions) {
            in_avail_set.erase(expr);
        }

        for (std::string expr : generatedExpressions) {
            in_avail_set.insert(expr);
        }
    }

    return modified;
}

bool CSEOptimizer::basicBlockBackwardsFindAndReplaceExpression(BasicBlock* basicBlock, std::string &expressionToFind, std::string &tempVar, BaseNode* beginBackwardsFromThisNode)
{
    //scan upwards, find the first occurrence of the expression z:=expr being generated by an instruction
    //replace it with a the temporary variable t:=expr
    //add a new MOV instruction after that to set z:=t
    //return true if the expression is found and replaced, false otherwise
    std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();

    instructions.reverse();

    std::list<BaseNode*>::iterator it2;
    if (beginBackwardsFromThisNode == nullptr) {
        it2 = instructions.begin();        //default start from the end of the basic block
    } else {
        it2 = std::find(instructions.begin(), instructions.end(), beginBackwardsFromThisNode);     //start backwards search from the beginBackwardsFromThisNode
    }

    for (; it2 != instructions.end(); ++it2) {    //reverse iterator
        BaseNode* instruction = *it2;
        std::set<std::string> generatedExpressions = instruction->getGeneratedExpressions();

        if (generatedExpressions.find(expressionToFind) != generatedExpressions.end()) {
            //expression found, EDIT

            //first just edit the existing expression node, and change the dest to the temp variable
            ExpressionNode* expressionNode = dynamic_cast<ExpressionNode*>(instruction);
            if (expressionNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not an ExpressionNode subclass");
            }
            std::string originalDest = expressionNode->getDest();
            expressionNode->setDest(tempVar);       //change the dest to the temp variable

            // after the current expression node t:=expr,
            // insert a new MOV instruction into the IR tree to set z:=t
            MovNode* movNode = new MovNode(originalDest, tempVar);
            it2 = basicBlock->insert_sandwich_instruction_node(it2, movNode);    //updates the iterator to point to the next instruction - don't really need it
            instructions.reverse();    //reverse back to original order before returning
            return true;      //expression found and replaced
        }
    }
    instructions.reverse();    //reverse back to original order before returning
    return false;       //expression not found - continue DFS search backwards
}

std::string CSEOptimizer::getNewTempVariableName()
{
    return "_t" + std::to_string(nextTempVariableCount++);
}