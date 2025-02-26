#include "LoopTiling.h"
#include "IrNestedLoopVisitor.h"
#include "EntryNode.h"
#include "LoopCondNode.h"
#include "MovNode.h"
#include "ExpressionNode.h"
#include "RelOpNode.h"
#include "LogicBinOpNode.h"
#include "LogicNotNode.h"
#include "ArithOpNode.h"
#include "EndBlockNode.h"
#include "TestNode.h"
#include "IfElseNode.h"
#include "DeclareNode.h"
#include <iostream>
#include <memory>

LoopTiling::LoopTiling(std::shared_ptr<EntryNode> entryNode, int nextLoopLabelNumber, int nextIfLabelNumber, int tempVariableCount, std::unordered_map<std::string, std::string> irDatatypeMap): entryNode(entryNode), loopLabelNumber(nextLoopLabelNumber), ifLabelNumber(nextIfLabelNumber), tempVariableCount(tempVariableCount), irDatatypeMap(irDatatypeMap) {

}

std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>, LoopTiling::LoopComparator> LoopTiling::getPerfectlyNestedLoops() {

    //Note (actually implemented using a visitor)
    //visit all the nodes in the graph, in dfs (left to right order)
    //if we see a loop cond node, and it is the outer loop, add it to the map, with an empty vector
    //if we see a loop cond node, and it is the inner loop, add it to the vector of the outer loop
    //until we exit that outer loop (keep track of how many layers of LOOPS we are in and ENDLOOPS using a stack)

    IrNestedLoopVisitor nestedLoopVisitor;
    entryNode->accept(nestedLoopVisitor);
    std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>> perfectlyNestedLoops = nestedLoopVisitor.getPerfectlyNestedLoops();

    //copy over the map to the loopCondNodesToTile map (which has a custom comparator)
    for (auto &keyValuePair : perfectlyNestedLoops) {
        loopCondNodesToTile[keyValuePair.first] = keyValuePair.second;
    }

    // for (auto &keyValuePair : loopCondNodesToTile) {
    //     std::cerr << "Outer loop: " << keyValuePair.first->getLabelNumber() << std::endl;
    //     for (auto &innerLoop : keyValuePair.second) {
    //         std::cerr << "  Inner loop: " << innerLoop->getLabelNumber() << std::endl;
    //     }
    // }

    return loopCondNodesToTile;
}

std::unordered_map<std::string, std::string> LoopTiling::getUpdatedIRDatatypeMap() const {
    return irDatatypeMap;
}   

bool LoopTiling::runLoopTiling() {

    //get the perfectly nested loops
    getPerfectlyNestedLoops();

    for (auto &keyValuePair : loopCondNodesToTile) {
        std::shared_ptr<LoopCondNode> originalOuterLoop = keyValuePair.first;
        std::shared_ptr<EndBlockNode> originalOuterLoopEndLoop = originalOuterLoop->getEndLoopNode();
        std::shared_ptr<BaseNode> originalOuterLoopEndLoopChild = originalOuterLoopEndLoop->getSingleChild();
        originalOuterLoopEndLoop->removeChild(*originalOuterLoopEndLoopChild);      //remove the stuff after all the nested loops in the original case
        std::vector<std::shared_ptr<LoopCondNode>> originalInnerLoops = keyValuePair.second;

        std::shared_ptr<LoopCondNode> newOuterLoop = getNewLoopCondNodeForTiledLoop(originalOuterLoop);
        std::shared_ptr<BaseNode> parent = originalOuterLoop->getParent();
        parent->removeChild(*originalOuterLoop);
        parent->addChild(newOuterLoop);
        newOuterLoop->getEndLoopNode()->addChild(originalOuterLoopEndLoopChild);      //add the stuff after all the nested loops in the original case to the new outer loop
        std::shared_ptr<LoopCondNode> previousLoopCondNode = newOuterLoop;

        for (auto &originalInnerLoop : originalInnerLoops) {
            std::shared_ptr<LoopCondNode> newInnerLoop = getNewLoopCondNodeForTiledLoop(originalInnerLoop);
            nestLoopCondNodes(previousLoopCondNode, newInnerLoop);
            previousLoopCondNode = newInnerLoop;
        }

        nestLoopCondNodes(previousLoopCondNode, originalOuterLoop);
    }

    return true;

}

std::shared_ptr<LoopCondNode> LoopTiling::getNewLoopCondNodeForTiledLoop(std::shared_ptr<LoopCondNode> loopCondNode) {

    //get the original variable, init and condition bounds
    std::shared_ptr<MovNode> initNodeCast = std::dynamic_pointer_cast<MovNode>(loopCondNode->getInitNode());
    if (!initNodeCast) {
        throw std::runtime_error("Loop tiling could not be performed due to an invalid cast for the init node.");
    }
    std::string originalLoopVariable = initNodeCast->getDest();
    std::string originalLowerBound = initNodeCast->getSrc();
    std::string originalUpperBound;

    //the condition is either a RelOpNode or a LogicBinOpNode or a LogicNotNode (since only those expressions give boolean results for bounds checking)
    std::shared_ptr<BaseNode> condNode = loopCondNode->getCondNode();
    if (std::dynamic_pointer_cast<RelOpNode>(condNode)) {
        std::shared_ptr<RelOpNode> relOpNodeCast = std::dynamic_pointer_cast<RelOpNode>(condNode);
        originalUpperBound = relOpNodeCast->getSrc2();
    } else if (std::dynamic_pointer_cast<LogicBinOpNode>(condNode)) {
        std::shared_ptr<LogicBinOpNode> logicBinOpNodeCast = std::dynamic_pointer_cast<LogicBinOpNode>(condNode);
        originalUpperBound = logicBinOpNodeCast->getSrc2();
    } else if (std::dynamic_pointer_cast<LogicNotNode>(condNode)) {
        std::shared_ptr<LogicNotNode> logicNotNodeCast = std::dynamic_pointer_cast<LogicNotNode>(condNode);
        originalUpperBound = logicNotNodeCast->getSrc();
    } else {
        throw std::runtime_error("Loop tiling could not be performed due to an invalid cast for the condition node.");
    }

    //create new program variables for the tiled loop
    //e.g. for loop variable i, create _ii and _imax 
    std::string newLoopVariable = "_" + originalLoopVariable + originalLoopVariable;    //duplicate the original loop variable name and add _ to it
    std::string newUpperBoundVariable = "_" + originalLoopVariable + "max";    //add _ and max before and after the original loop variable name

    ///ADD DECLARATIONS for the new variables (if they are indeed new! E.g. other loops may have already used them) to the entry node, and add datatype to irDatatypeMap
    std::string originalLoopVariableDatatype = irDatatypeMap[originalLoopVariable];
    if (std::find(tilingVariablesAdded.begin(), tilingVariablesAdded.end(), newLoopVariable) == tilingVariablesAdded.end()) {
        entryNode->insertSandwichChild(std::make_shared<DeclareNode>(originalLoopVariableDatatype, newLoopVariable));
        tilingVariablesAdded.push_back(newLoopVariable);
        irDatatypeMap[newLoopVariable] = originalLoopVariableDatatype;
    }
    if (std::find(tilingVariablesAdded.begin(), tilingVariablesAdded.end(), newUpperBoundVariable) == tilingVariablesAdded.end()) {
        entryNode->insertSandwichChild(std::make_shared<DeclareNode>(originalLoopVariableDatatype, newUpperBoundVariable));
        tilingVariablesAdded.push_back(newUpperBoundVariable);
        irDatatypeMap[newUpperBoundVariable] = originalLoopVariableDatatype;
    }
    
    //create the new loopCondNode
    //need to create it with a new loop label number no one else in program is using - pass this out of the parser
    std::string labelNumber = std::to_string(loopLabelNumber++);
    std::shared_ptr<LoopCondNode> newLoopCondNode = std::make_shared<LoopCondNode>(labelNumber);

    //loop init node
    std::shared_ptr<MovNode> newInitNode = std::make_shared<MovNode>(newLoopVariable, originalLowerBound);
    newLoopCondNode->addChild(newInitNode);

    //loop condition node
    std::string newCondTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<RelOpNode> newUpperBoundNode = std::make_shared<RelOpNode>("GT", newCondTempVar, newLoopVariable, originalUpperBound);
    newLoopCondNode->addChild(newUpperBoundNode);

    std::shared_ptr<TestNode> newUpperBoundTestNode = std::make_shared<TestNode>(newCondTempVar);
    newUpperBoundNode->addChild(newUpperBoundTestNode);

    ////now create the new loop body - the 'if' for the min(original upper bound, ii + tile size -1)
    std::string newIfLabelNumber = std::to_string(ifLabelNumber++);
    std::shared_ptr<IfElseNode> newLoopBodyIfNode = std::make_shared<IfElseNode>(newIfLabelNumber);

    std::string newAddTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newAddNode1 = std::make_shared<ArithOpNode>("ADD", newAddTempVar, newLoopVariable, TILE_SIZE);
    std::string newSubTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newSubNode1 = std::make_shared<ArithOpNode>("SUB", newSubTempVar, newAddTempVar, "1");
    std::string newGTTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<RelOpNode> newGTNode1 = std::make_shared<RelOpNode>("GT", newGTTempVar, newSubTempVar, originalUpperBound);
    std::shared_ptr<TestNode> newTestNode1 = std::make_shared<TestNode>(newGTTempVar);
    newLoopBodyIfNode->addChild(newAddNode1);
    newAddNode1->addChild(newSubNode1);
    newSubNode1->addChild(newGTNode1);
    newGTNode1->addChild(newTestNode1);
    

    std::shared_ptr<MovNode> newMovNode2 = std::make_shared<MovNode>(newUpperBoundVariable, originalUpperBound);
    std::shared_ptr<EndBlockNode> newEndBlock2 = std::make_shared<EndBlockNode>("ENDTHEN");
    newLoopBodyIfNode->addChild(newMovNode2);
    newMovNode2->addChild(newEndBlock2);

    std::string newAddTempVar2 = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newAddNode3 = std::make_shared<ArithOpNode>("ADD", newAddTempVar2, newLoopVariable, TILE_SIZE);
    std::string newSubTempVar2 = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newSubNode3 = std::make_shared<ArithOpNode>("SUB", newSubTempVar2, newAddTempVar2, "1");
    std::shared_ptr<MovNode> newMovNode3 = std::make_shared<MovNode>(newUpperBoundVariable, newSubTempVar2);
    std::shared_ptr<EndBlockNode> newEndBlock3 = std::make_shared<EndBlockNode>("ENDTHEN");
    newLoopBodyIfNode->addChild(newAddNode3);
    newAddNode3->addChild(newSubNode3);
    newSubNode3->addChild(newMovNode3);
    newMovNode3->addChild(newEndBlock3);
    
    std::shared_ptr<EndBlockNode> newEndBlock4 = std::make_shared<EndBlockNode>("ENDIF");
    std::shared_ptr<EndBlockNode> newEndBlock5 = std::make_shared<EndBlockNode>("ENDBODY");    //of the new LoopCondNode we are creating
    newLoopBodyIfNode->addChild(newEndBlock4);
    newEndBlock4->addChild(newEndBlock5);

    //end of creating the new loop body (i.e. the if node)
    newLoopCondNode->addChild(newLoopBodyIfNode);

    //loop step node
    std::string newStepTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newStepAddNode = std::make_shared<ArithOpNode>("ADD", newStepTempVar, newLoopVariable, TILE_SIZE);
    newLoopCondNode->addChild(newStepAddNode);

    std::shared_ptr<MovNode> newStepMovNode = std::make_shared<MovNode>(newLoopVariable, newStepTempVar);
    newStepAddNode->addChild(newStepMovNode);
    
    //loop end node
    std::shared_ptr<EndBlockNode> newEndLoopNode = std::make_shared<EndBlockNode>("ENDLOOP");    //of the new LoopCondNode we are creating
    newLoopCondNode->addChild(newEndLoopNode);


    ///MODIFY the original loopCondNode to have the correct bounds of _ii and _imax
    modifyOriginalLoopCondNodeBounds(loopCondNode, newLoopVariable, newUpperBoundVariable);

    return newLoopCondNode;
}

std::shared_ptr<EndBlockNode> LoopTiling::getLoopCondNodeEndBodyNode(std::shared_ptr<LoopCondNode> loopCondNode) {
    std::shared_ptr<IfElseNode> ifNode = std::dynamic_pointer_cast<IfElseNode>(loopCondNode->getBodyNode());
    std::shared_ptr<EndBlockNode> endIfNode = std::dynamic_pointer_cast<EndBlockNode>(ifNode->getEndIfNode());
    std::shared_ptr<EndBlockNode> endBodyNode = std::dynamic_pointer_cast<EndBlockNode>(endIfNode->getSingleChild());
    return endBodyNode;
}

void LoopTiling::nestLoopCondNodes(std::shared_ptr<LoopCondNode> outerLoopCondNode, std::shared_ptr<LoopCondNode> innerLoopCondNode) {
    std::shared_ptr<EndBlockNode> endBodyNode = getLoopCondNodeEndBodyNode(outerLoopCondNode);
    std::shared_ptr<EndBlockNode> endIfNode = std::dynamic_pointer_cast<EndBlockNode>(endBodyNode->getParent());
    endIfNode->removeChild(*endBodyNode);

    //sandwich the inner loopCondNode before the endBodyNode
    endIfNode->addChild(innerLoopCondNode);
    std::shared_ptr<EndBlockNode> innerEndLoopNode = innerLoopCondNode->getEndLoopNode();
    innerEndLoopNode->addChild(endBodyNode);
}

void LoopTiling::modifyOriginalLoopCondNodeBounds(std::shared_ptr<LoopCondNode> loopCondNode, std::string newLowerBoundVariable, std::string newUpperBoundVariable) {
    std::shared_ptr<MovNode> initNode = std::dynamic_pointer_cast<MovNode>(loopCondNode->getInitNode());
    if (!initNode) {
        throw std::runtime_error("Loop tiling could not be performed due to an invalid cast for the init node.");
    }
    initNode->setSrc(newLowerBoundVariable);

    std::shared_ptr<RelOpNode> condNode = std::dynamic_pointer_cast<RelOpNode>(loopCondNode->getCondNode());
    if (!condNode) {
        throw std::runtime_error("Loop tiling could not be performed due to an invalid cast for the condition node.");
    }
    condNode->setSrc2(newUpperBoundVariable);
}

std::string LoopTiling::getNewTempVariableName(std::string datatype)
{
    std::string tempVar = "_t" + std::to_string(tempVariableCount++);
    irDatatypeMap[tempVar] = datatype;
    return tempVar;
}