#include "CSEOptimizer.h"
#include "SimpleNode.h"
#include "ExpressionNode.h"
#include "MovNode.h"
#include "DeclareNode.h"
#include "PropagationOptimizer.h"
#include <set>
#include <stack>
#include "EntryNode.h"


CSEOptimizer::CSEOptimizer(std::shared_ptr<EntryNode> entryNode, std::unordered_map<std::string, std::string> irDatatypeMap, int nextProgramTempVariableCount) : entryNode(entryNode), irDatatypeMap(irDatatypeMap), nextProgramTempVariableCount(nextProgramTempVariableCount) {
}

std::unordered_map<std::string, std::string> CSEOptimizer::getUpdatedIRDatatypeMap() const {
    return irDatatypeMap;
}

bool CSEOptimizer::runCommonSubexpressionElimination()
{
    // need to reinitialise the basic blocks and avail sets each time, since it might have changed since the last iteration
    startBasicBlock = AnalysisTools::drawFlowgraph(entryNode);

    // Analysis: AVAIL
    AVAIL avail(startBasicBlock);
    nodeAvailSets = avail.getNodeInDataFlowSets();
    basicBlocks = avail.getBasicBlocksUsed();

    //get the nodes in flowgraph order
    std::vector<std::shared_ptr<BaseNode>> originalTreeNodes = AnalysisTools::getIRNodesInFlowgraphOrder(basicBlocks);

    // Transformation: Remove common subexpressions
    bool removed = false;
    for (std::shared_ptr<BaseNode> node : originalTreeNodes) {
        removed |= removeCommonSubexpressionsForNode(node);
        //don't need to redraw after each node, only need to redraw between CSE calls, and also before going backwards in the flowgraph (between FIRST and SECOND)
    }
    return removed;
}

bool CSEOptimizer::iterateCSE_CopyPropagation()
{
    bool modified = true;

    while (modified) {
        modified = runCommonSubexpressionElimination();
        PropagationOptimizer propagationOptimizer(entryNode);
        modified |= propagationOptimizer.runCopyPropagation();
    }
    return modified;
}

int CSEOptimizer::getNextProgramTempVariableCount() const
{
    return nextProgramTempVariableCount;
}

bool CSEOptimizer::removeCommonSubexpressionsForNode(std::shared_ptr<BaseNode> instruction) {

    std::set<std::string> in_avail_set = nodeAvailSets[instruction];   //get the in_avail set for the instruction
    std::set<std::string> generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(instruction);
        
    if (generatedExpressions.empty()) {
        //the instruction does not generate any expressions
        return false;
    }

    //otherwise, there is at least one expression generated
    assert(generatedExpressions.size() == 1);     //should just be one expression that is generated, which we need to handle if it is already available
    std::string expr = *generatedExpressions.begin();

    if (in_avail_set.find(expr) != in_avail_set.end()) {    
        //current instruction DOES generates an available expression!
        //handle with CSE transformation!
        
        //the instruction should be ArithmeticNode, LogicBinNode, LogicNotNode, or RelOpNode
        //these are all SimpleNode subclasses!!
        std::shared_ptr<SimpleNode> simpleNode = std::dynamic_pointer_cast<SimpleNode>(instruction);
        if (simpleNode == nullptr) {
            throw std::runtime_error("ERROR while handling CSE: Instruction is not a SimpleNode subclass");
        }

        ///FIRST: replace the current instruction y:=e
        //with a MOV instruction with a new temporary variable y:= t   
        assert(simpleNode->getDefinedVariables().size() == 1);
        std::string destVar = *simpleNode->getDefinedVariables().begin();  //the variable defined, i.e. y

        std::string IRdatatype = irDatatypeMap[destVar];
        std::string tempVar = addNewProgramTempVariable(IRdatatype);     //Also adds a declaration!! - temporary t should be the same datatype as the y
        
        std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(destVar,tempVar);
        simpleNode->replaceCurrentNodeInIRTree(movNode);   //WARNING: simple node is no longer in the IR tree!!!

        ///REDRAW: need to redraw the flowgraph so the movNode is in the flowgraph, and any further expression eliminations see that this y:=e no longer exists
        startBasicBlock = AnalysisTools::drawFlowgraph(entryNode);
        basicBlocks = AnalysisTools::getBasicBlocks(startBasicBlock);
        std::map<std::weak_ptr<BaseNode>, std::shared_ptr<BasicBlock>, std::owner_less<std::weak_ptr<BaseNode>>> flowgraphNodeToBasicBlockMap = AnalysisTools::getFlowgraphNodeToBasicBlockMap(basicBlocks);
        std::shared_ptr<BasicBlock> currentBasicBlock = flowgraphNodeToBasicBlockMap[movNode];

        ///SECOND: for every backwards control flow, find the first occurrence of the expression, and EDIT
        // do a DFS backwards from the current basic block as the start node
            //need function to go inside each basic block
        std::stack<std::shared_ptr<BasicBlock>> toExploreStack;
        toExploreStack.push(currentBasicBlock);
        std::set<std::shared_ptr<BasicBlock>> seen;
        seen.insert(currentBasicBlock);

        while (!toExploreStack.empty()) {
            std::shared_ptr<BasicBlock> current = toExploreStack.top();
            toExploreStack.pop();
            //go inside each basic block

            bool expressionFound = false;
            if (current == currentBasicBlock) {      //only for the first basic block, start backwards search from the current instruction
                //beginBackwardsFromThisNode is the current instruction node (which was replaced with the movNode)
                expressionFound = basicBlockBackwardsFindAndReplaceExpression(current, expr, tempVar, movNode);   //start backwards from the movNode which has just replaced the current instruction
            } else {             //for all other basic blocks, start backwards search from the end of the basic block
                expressionFound = basicBlockBackwardsFindAndReplaceExpression(current, expr, tempVar);
            }

            if (!expressionFound){    //then we add the predecessors to the stack 
                std::vector<std::weak_ptr<BasicBlock>> predecessors = current->get_predecessors();
                //check that there should be at least one predecessor, otherwise we have hit the entry instruction node on a backwards path
                if (predecessors.empty()) {
                    throw std::runtime_error("ERROR while handling CSE: expression " + expr + " not found on a backwards path");
                }
                for (std::weak_ptr<BasicBlock> predecessor_weak_ptr : predecessors) {
                    if (predecessor_weak_ptr.expired()) {
                        throw std::runtime_error("ERROR while handling CSE: predecessor is expired (should not happen)");
                    }
                    std::shared_ptr<BasicBlock> predecessor = predecessor_weak_ptr.lock();
                    if (seen.find(predecessor) == seen.end()) {     //if it is not already seen
                        toExploreStack.push(predecessor);
                        seen.insert(predecessor);
                    }
                }
            }
        }
        return true;
    }
    return false;
}


bool CSEOptimizer::basicBlockBackwardsFindAndReplaceExpression(std::shared_ptr<BasicBlock> basicBlock, std::string &expressionToFind, std::string &tempVar, std::shared_ptr<BaseNode> beginBackwardsFromThisNode)
{
    //scan upwards, find the first occurrence of the expression z:=expr being generated by an instruction
    //replace it with a the temporary variable t:=expr
    //add a new MOV instruction after that to set z:=t
    //return true if the expression is found and replaced, false otherwise
    std::list<std::weak_ptr<BaseNode>> instructions = basicBlock->get_instructions_copy();

    instructions.reverse();

    std::list<std::weak_ptr<BaseNode>>::iterator it2;
    if (beginBackwardsFromThisNode == nullptr) {
        it2 = instructions.begin();        //default start from the end of the basic block
    } else {
        it2 = std::find_if(instructions.begin(), instructions.end(), [beginBackwardsFromThisNode](std::weak_ptr<BaseNode> node) {
            return node.lock() == beginBackwardsFromThisNode;
        });     //start backwards search from the beginBackwardsFromThisNode
    }

    for (; it2 != instructions.end(); ++it2) {    //reverse iterator
        std::shared_ptr<BaseNode> instruction = it2->lock();
        std::set<std::string> generatedExpressions = AnalysisTools::getGeneratedExpressionsAtNode(instruction);

        if (generatedExpressions.find(expressionToFind) != generatedExpressions.end()) {
            //expression found, EDIT

            //first just edit the existing expression node, and change the dest to the temp variable
            std::shared_ptr<ExpressionNode> expressionNode = std::dynamic_pointer_cast<ExpressionNode>(instruction);
            if (expressionNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not an ExpressionNode subclass");
            }
            std::string originalDest = expressionNode->getDest();
            expressionNode->setDest(tempVar);       //change the dest to the temp variable

            // after the current expression node t:=expr,
            // insert a new MOV instruction into the IR tree to set z:=t
            std::shared_ptr<MovNode> movNode = std::make_shared<MovNode>(originalDest, tempVar);
            std::shared_ptr<SimpleNode> simpleNode = std::dynamic_pointer_cast<SimpleNode>(instruction);
            if (simpleNode == nullptr) {
                throw std::runtime_error("ERROR while handling CSE: Instruction is not a SimpleNode subclass");
            }
            simpleNode->insertSandwichChild(movNode);
            instructions.reverse();    //reverse back to original order before returning and inserting to the start of the BasicBlock
            return true;      //expression found and replaced
        }
    }
    instructions.reverse();    //reverse back to original order before returning
    return false;       //expression not found - continue DFS search backwards
}

std::string CSEOptimizer::addNewProgramTempVariable(std::string IRdatatype)
{
    //increment nextProgramTempVariableCount after creating the new program temp variable
    std::string programTempVar = "_s" + std::to_string(nextProgramTempVariableCount++);

    //add a declare statement to the start of the program
    ///QUESTION: what datatype should we use for the program temp variable?
    std::shared_ptr<DeclareNode> declareTempVarNode = std::make_shared<DeclareNode>(IRdatatype, programTempVar);

    //add the new variable to the irDatatypeMap
    irDatatypeMap[programTempVar] = IRdatatype;

    //insert the declare statement at the start of the program (after the entry node)
    entryNode->insertSandwichChild(declareTempVarNode);
    
    return programTempVar;
}