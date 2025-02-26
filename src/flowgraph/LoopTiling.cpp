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
    std::string originalLoopVariable;
    std::string originalLowerBound;
    std::shared_ptr<BaseNode> currentInitNode = loopCondNode->getInitNode();   //assuming all the initialisation is linear code
    std::shared_ptr<BaseNode> currentCondNode = loopCondNode->getCondNode();

    //finding the original loop variable and lower bound, by going to the bottom of the init node (i.e. a MovNode)
    std::shared_ptr<SimpleNode> currentInitSimpleNode = std::dynamic_pointer_cast<SimpleNode>(currentInitNode);
    while (currentInitSimpleNode && !std::dynamic_pointer_cast<MovNode>(currentInitSimpleNode)) {
        //check if the next node is the MovNode we are looking for
        currentInitSimpleNode = std::dynamic_pointer_cast<SimpleNode>(currentInitSimpleNode->getSingleChild());
    }
    if (!currentInitSimpleNode) {
        throw std::runtime_error("Loop tiling could not be performed due to an invalid cast for the init node.");
    }
    //cast it to a MovNode
    std::shared_ptr<MovNode> currentInitMovNode = std::dynamic_pointer_cast<MovNode>(currentInitSimpleNode);
    originalLoopVariable = currentInitMovNode->getDest();
    originalLowerBound = currentInitMovNode->getSrc();


    //create new program VARIABLES for the tiled loop
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


    //get the original upper bound
    std::string originalUpperBound = getOriginalUpperBound(loopCondNode);

    ///MODIFY the original loopCondNode to have the correct bounds of _ii and _imax (in fact have to CREATE new nodes and insert them into the correct child position)
    std::shared_ptr<MovNode> newInitNode = std::make_shared<MovNode>(originalLoopVariable, newLoopVariable);
    std::string newCondTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<RelOpNode> newCondNode = std::make_shared<RelOpNode>("GT", newCondTempVar, originalLoopVariable, newUpperBoundVariable);
    std::shared_ptr<TestNode> newCondTestNode = std::make_shared<TestNode>(newCondTempVar);
    newCondNode->addChild(newCondTestNode);
    //have to detach the init node and condition node from the loopCondNode first
    loopCondNode->removeChild(*currentInitNode);
    loopCondNode->removeChild(*currentCondNode);
    //then add the new nodes in the correct positions
    loopCondNode->addChildAtIndex(newInitNode, 0);
    loopCondNode->addChildAtIndex(newCondNode, 1);

    
    ///CREATE the new loopCondNode
    //need to create it with a new loop label number no one else in program is using - pass this out of the parser
    std::string labelNumber = std::to_string(loopLabelNumber++);
    std::shared_ptr<LoopCondNode> newLoopCondNode = std::make_shared<LoopCondNode>(labelNumber);

    //USE the loop init node and condition node from the original loopCondNode
    newLoopCondNode->addChild(currentInitNode);
    newLoopCondNode->addChild(currentCondNode);
    //but modify any variables from i to ii
    currentInitMovNode->setDest(newLoopVariable);
    modifyUpperBoundCondNode(newLoopCondNode, newLoopVariable);


    //make a COPY of the upper bound condition node (if its an arithmetic EXPRESSION - we need to copy that into the min() checking thing)
    std::shared_ptr<ArithOpNode> currentArithCondNode = std::dynamic_pointer_cast<ArithOpNode>(currentCondNode);
    bool origUpperBoundIsArithExpr = currentArithCondNode != nullptr;    //this flag for more complex case

    ////NOW create the new loop body - the 'if' for the min(original upper bound, ii + tile size -1)
    std::string newIfLabelNumber = std::to_string(ifLabelNumber++);
    std::shared_ptr<IfElseNode> newLoopBodyIfNode = std::make_shared<IfElseNode>(newIfLabelNumber);

    //condition checking
    std::string newAddTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newAddNode1 = std::make_shared<ArithOpNode>("ADD", newAddTempVar, newLoopVariable, TILE_SIZE);
    std::string newSubTempVar = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newSubNode1 = std::make_shared<ArithOpNode>("SUB", newSubTempVar, newAddTempVar, "1");
    newLoopBodyIfNode->addChild(newAddNode1);
    newAddNode1->addChild(newSubNode1);

    std::string newGTTempVar= getNewTempVariableName(originalLoopVariableDatatype);;
    std::shared_ptr<RelOpNode> newGTNode1;
    if (origUpperBoundIsArithExpr) {          //need to add a more complex condition node for the original upper bound
        std::pair<std::shared_ptr<ArithOpNode>, std::shared_ptr<ArithOpNode>> arithCondNodeCopy = getCopyOfArithCondition(currentArithCondNode);
        newSubNode1->addChild(arithCondNodeCopy.first);
        assert(originalUpperBound == arithCondNodeCopy.second->getDest());      //these should be the same - referenced in some way
        newGTNode1 = std::make_shared<RelOpNode>("GT", newGTTempVar, newSubTempVar, originalUpperBound);
        arithCondNodeCopy.second->addChild(newGTNode1);
    }
    else {
        newGTNode1 = std::make_shared<RelOpNode>("GT", newGTTempVar, newSubTempVar, originalUpperBound);
        newSubNode1->addChild(newGTNode1);
    }
    std::shared_ptr<TestNode> newTestNode1 = std::make_shared<TestNode>(newGTTempVar);
    newGTNode1->addChild(newTestNode1);

    
    //Then branch
    std::shared_ptr<MovNode> newMovNode2;
    if (origUpperBoundIsArithExpr){
        std::pair<std::shared_ptr<ArithOpNode>, std::shared_ptr<ArithOpNode>> arithCondNodeCopy = getCopyOfArithCondition(currentArithCondNode);
        newLoopBodyIfNode->addChild(arithCondNodeCopy.first);
        newMovNode2 = std::make_shared<MovNode>(newUpperBoundVariable, arithCondNodeCopy.second->getDest());
        arithCondNodeCopy.second->addChild(newMovNode2);
    }
    else {
        newMovNode2 = std::make_shared<MovNode>(newUpperBoundVariable, originalUpperBound);
        newLoopBodyIfNode->addChild(newMovNode2);
    }
    std::shared_ptr<EndBlockNode> newEndBlock2 = std::make_shared<EndBlockNode>("ENDTHEN");
    newMovNode2->addChild(newEndBlock2);

    //else branch
    std::string newAddTempVar2 = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newAddNode3 = std::make_shared<ArithOpNode>("ADD", newAddTempVar2, newLoopVariable, TILE_SIZE);
    std::string newSubTempVar2 = getNewTempVariableName(originalLoopVariableDatatype);
    std::shared_ptr<ArithOpNode> newSubNode3 = std::make_shared<ArithOpNode>("SUB", newSubTempVar2, newAddTempVar2, "1");
    std::shared_ptr<MovNode> newMovNode3 = std::make_shared<MovNode>(newUpperBoundVariable, newSubTempVar2);
    std::shared_ptr<EndBlockNode> newEndBlock3 = std::make_shared<EndBlockNode>("ENDELSE");
    newLoopBodyIfNode->addChild(newAddNode3);
    newAddNode3->addChild(newSubNode3);
    newSubNode3->addChild(newMovNode3);
    newMovNode3->addChild(newEndBlock3);

    //end of the if node
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

std::pair<std::shared_ptr<ArithOpNode>, std::shared_ptr<ArithOpNode>> LoopTiling::getCopyOfArithCondition(std::shared_ptr<ArithOpNode> startArithOpNode) {

    std::shared_ptr<ArithOpNode> currentArithCondNodeCopyTop;
    std::shared_ptr<ArithOpNode> currentArithCondNodeCopyBottom;

    currentArithCondNodeCopyTop = std::dynamic_pointer_cast<ArithOpNode>(startArithOpNode->cloneContent());   //clone current arithmetic node
    currentArithCondNodeCopyBottom = currentArithCondNodeCopyTop;   //might be the same node

    //get its child node - if its another arithmetic node, continue to copy it and add to currentArithCondNodeCopy
    std::shared_ptr<ArithOpNode> arithCondChild = std::dynamic_pointer_cast<ArithOpNode>(startArithOpNode->getSingleChild());
    while (arithCondChild) {
        std::shared_ptr<ArithOpNode> arithCondChildCopy = std::dynamic_pointer_cast<ArithOpNode>(arithCondChild->cloneContent());
        currentArithCondNodeCopyTop->addChild(arithCondChildCopy);

        //get the next child node
        currentArithCondNodeCopyBottom = arithCondChildCopy;    //keep track of the bottom node, so we can attach stuff to it later
        arithCondChild = std::dynamic_pointer_cast<ArithOpNode>(arithCondChild->getSingleChild());
    }

    return std::make_pair(currentArithCondNodeCopyTop, currentArithCondNodeCopyBottom);
}

std::string LoopTiling::getOriginalUpperBound(std::shared_ptr<LoopCondNode> loopCondNode) {
    std::string originalUpperBound;

    std::shared_ptr<BaseNode> condNode = loopCondNode->getCondNode();
    std::shared_ptr<SimpleNode> simpleNode = std::dynamic_pointer_cast<SimpleNode>(condNode);
    while (simpleNode && simpleNode->getChildren().size() > 0) {
        condNode = simpleNode->getSingleChild();
        simpleNode = std::dynamic_pointer_cast<SimpleNode>(condNode);
    }
    //condNode is now the TestNode
    condNode = condNode->getParent();    //get the 2nd to last node : should be RelOpNode GT

    if (std::dynamic_pointer_cast<RelOpNode>(condNode)) {
        std::shared_ptr<RelOpNode> relOpNodeCast = std::dynamic_pointer_cast<RelOpNode>(condNode);
        originalUpperBound = relOpNodeCast->getSrc2();
    } else{
        throw std::runtime_error("Loop tiling could not be performed due to upper bound not being a RelOpNode. " + condNode->getText());
    }
    return originalUpperBound;
}

void LoopTiling::modifyUpperBoundCondNode(std::shared_ptr<LoopCondNode> loopCondNode, std::string newLoopVariable) {

    std::shared_ptr<BaseNode> condNode = loopCondNode->getCondNode();
    std::shared_ptr<SimpleNode> simpleNode = std::dynamic_pointer_cast<SimpleNode>(condNode);
    while (simpleNode && simpleNode->getChildren().size() > 0) {
        condNode = simpleNode->getSingleChild();
        simpleNode = std::dynamic_pointer_cast<SimpleNode>(condNode);
    }
    //condNode is now the TestNode
    condNode = condNode->getParent();    //get the 2nd to last node : should be RelOpNode GT
    if (std::dynamic_pointer_cast<RelOpNode>(condNode)) {
        std::shared_ptr<RelOpNode> relOpNodeCast = std::dynamic_pointer_cast<RelOpNode>(condNode);
        relOpNodeCast->setSrc1(newLoopVariable);
    } else {
        throw std::runtime_error("Loop tiling could not be performed due to an invalid cast for the condition node."+condNode->getText());
    }

}

std::string LoopTiling::getNewTempVariableName(std::string datatype)
{
    std::string tempVar = "_t" + std::to_string(tempVariableCount++);
    irDatatypeMap[tempVar] = datatype;
    return tempVar;
}