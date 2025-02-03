#include "SimplificationOptimisations.h"
#include <iostream>
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

