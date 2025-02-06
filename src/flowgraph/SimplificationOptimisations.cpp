#include "SimplificationOptimisations.h"
#include <iostream>
#include "DeclareNode.h"

void SimplificationOptimisations::removeAllEmptyControlFlowConstructs(std::shared_ptr<BaseNode> root) {

    bool runRemoveEmptyStatements = true;
    while (runRemoveEmptyStatements) {
        runRemoveEmptyStatements = removeEmptyStatements(root);
    }

}

bool SimplificationOptimisations::removeEmptyStatements(std::shared_ptr<BaseNode> root) {
    //check the nodes in the tree
    //if a node is an IfNode, IfElseNode, LoopNode or LoopCondNode and has no body, remove it

    //dfs of the tree, check each node

    std::shared_ptr<BaseNode> currentNode = root;

    //it is a tree, so won't visit the same node twice
    std::stack<std::shared_ptr<BaseNode>> nodesToVisit;
    nodesToVisit.push(currentNode);

    bool atLeastOneNodeDeleted = false;
    while (!nodesToVisit.empty()) {
        currentNode = nodesToVisit.top();
        nodesToVisit.pop();

        //check if the node is an IfNode, IfElseNode, LoopNode or LoopCondNode and has no body, remove it
        std::shared_ptr<BaseNode> attachedChild = nullptr;
        bool nodeDeleted = false;

        if (std::dynamic_pointer_cast<IfNode>(currentNode) != nullptr) {     //if it is an IfNode, check if the THEN statement is empty
            std::shared_ptr<IfNode> ifNode = std::dynamic_pointer_cast<IfNode>(currentNode);
            if (std::dynamic_pointer_cast<EndBlockNode>(ifNode->getChildren()[1]) != nullptr && std::dynamic_pointer_cast<EndBlockNode>(ifNode->getChildren()[1])->getText() == "ENDTHEN") {
                attachedChild = ifNode->removeCurrentNodeFromIRTree();
                nodeDeleted = true;
            }
        }
        else if (std::dynamic_pointer_cast<IfElseNode>(currentNode) != nullptr) {
            std::shared_ptr<IfElseNode> ifElseNode = std::dynamic_pointer_cast<IfElseNode>(currentNode);
            if (std::dynamic_pointer_cast<EndBlockNode>(ifElseNode->getChildren()[1]) != nullptr && std::dynamic_pointer_cast<EndBlockNode>(ifElseNode->getChildren()[1])->getText() == "ENDTHEN"
                && std::dynamic_pointer_cast<EndBlockNode>(ifElseNode->getChildren()[2]) != nullptr && std::dynamic_pointer_cast<EndBlockNode>(ifElseNode->getChildren()[2])->getText() == "ENDELSE") {
                attachedChild = ifElseNode->removeCurrentNodeFromIRTree();
                nodeDeleted = true;
            }
        }
        else if (std::dynamic_pointer_cast<LoopNode>(currentNode) != nullptr) {
            std::shared_ptr<LoopNode> loopNode = std::dynamic_pointer_cast<LoopNode>(currentNode);
            if (std::dynamic_pointer_cast<EndBlockNode>(loopNode->getChildren()[0]) != nullptr && std::dynamic_pointer_cast<EndBlockNode>(loopNode->getChildren()[0])->getText() == "ENDBODY") {
                attachedChild = loopNode->removeCurrentNodeFromIRTree();
                nodeDeleted = true;
            }
        }
        else if (std::dynamic_pointer_cast<LoopCondNode>(currentNode) != nullptr) {
            std::shared_ptr<LoopCondNode> loopCondNode = std::dynamic_pointer_cast<LoopCondNode>(currentNode);
            if (std::dynamic_pointer_cast<EndBlockNode>(loopCondNode->getChildren()[2]) != nullptr && std::dynamic_pointer_cast<EndBlockNode>(loopCondNode->getChildren()[2])->getText() == "ENDBODY") {
                attachedChild = loopCondNode->removeCurrentNodeFromIRTree();  
                nodeDeleted = true;
            }     
        }

        if (nodeDeleted) {          //if a node was deleted, so add its single child (which replaced that node) onto the stack
            ///WARNING: currentNode no longer exists!!!
            if (attachedChild != nullptr) {
                nodesToVisit.push(attachedChild);
            }
        }else{
            // no node was deleted, so add all children onto the stack
            for (std::shared_ptr<BaseNode> child : currentNode->getChildren()) {
                nodesToVisit.push(child);
            }
        }
         //continue dfs

        atLeastOneNodeDeleted = atLeastOneNodeDeleted || nodeDeleted;
    }

    return atLeastOneNodeDeleted;
}

void SimplificationOptimisations::removeUnusedDeclareStatements(std::shared_ptr<BaseNode> root) {
    //get all referenced variables in the program
    //then go through each node, if it is a declare statement and the variable is not referenced, remove it

    std::set<std::string> referencedVariables;
    std::set<std::shared_ptr<DeclareNode>> declareNodesInProgram;

    //visit the tree and get all referenced variables
    std::stack<std::shared_ptr<BaseNode>> nodesToVisit;
    nodesToVisit.push(root);

    while (!nodesToVisit.empty()) {
        std::shared_ptr<BaseNode> currentNode = nodesToVisit.top();
        nodesToVisit.pop();
        //add the node's referenced variables to the set
        std::set<std::string> currentNodeReferencedVariables = currentNode->getReferencedVariables();
        referencedVariables.insert(currentNodeReferencedVariables.begin(), currentNodeReferencedVariables.end());

        //add the node's children to the stack
        for (std::shared_ptr<BaseNode> child : currentNode->getChildren()) {
            nodesToVisit.push(child);
        }

        //if it is a declare statement, add the node to the set of declare nodes 
        if (std::dynamic_pointer_cast<DeclareNode>(currentNode) != nullptr) {
            declareNodesInProgram.insert(std::dynamic_pointer_cast<DeclareNode>(currentNode));
        }
    }

    //go through each declare node in the program, if it is not referenced, remove it
    for (std::shared_ptr<DeclareNode> declareNode : declareNodesInProgram) {
        if (referencedVariables.find(declareNode->getVar()) == referencedVariables.end()) {
            //declare node is not referenced, so remove it
            declareNode->removeCurrentNodeFromIRTree();
        }
    }
}
