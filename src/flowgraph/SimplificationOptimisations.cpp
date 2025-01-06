#include "SimplificationOptimisations.h"
#include <iostream>
void SimplificationOptimisations::removeAllEmptyControlFlowConstructs(BaseNode* root) {

    bool runRemoveEmptyStatements = true;
    while (runRemoveEmptyStatements) {
        runRemoveEmptyStatements = removeEmptyStatements(root);
    }

}

bool SimplificationOptimisations::removeEmptyStatements(BaseNode* root) {
    //check the nodes in the tree
    //if a node is an IfNode, IfElseNode, LoopNode or LoopCondNode and has body, remove it

    //dfs of the tree, check each node

    BaseNode* currentNode = root;

    //it is a tree, so won't visit the same node twice
    std::stack<BaseNode*> nodesToVisit;
    nodesToVisit.push(currentNode);

    bool atLeastOneNodeDeleted = false;
    while (!nodesToVisit.empty()) {
        currentNode = nodesToVisit.top();
        nodesToVisit.pop();

        //check if the node is an IfNode, IfElseNode, LoopNode or LoopCondNode and has body, remove it
        BaseNode* attachedChild = nullptr;
        bool nodeDeleted = false;

        if (dynamic_cast<IfNode*>(currentNode) != nullptr) {     //if it is an IfNode, check if the THEN statement is empty
            IfNode* ifNode = dynamic_cast<IfNode*>(currentNode);
            if (dynamic_cast<EndBlockNode*>(ifNode->getChildren()[1]) != nullptr && dynamic_cast<EndBlockNode*>(ifNode->getChildren()[1])->getText() == "ENDTHEN") {
                attachedChild = ifNode->removeCurrentNodeFromIRTree();
                nodeDeleted = true;
            }
        }
        else if (dynamic_cast<IfElseNode*>(currentNode)) {
            IfElseNode* ifElseNode = dynamic_cast<IfElseNode*>(currentNode);
            if (dynamic_cast<EndBlockNode*>(ifElseNode->getChildren()[1]) != nullptr && dynamic_cast<EndBlockNode*>(ifElseNode->getChildren()[1])->getText() == "ENDTHEN"
                && dynamic_cast<EndBlockNode*>(ifElseNode->getChildren()[2]) != nullptr && dynamic_cast<EndBlockNode*>(ifElseNode->getChildren()[2])->getText() == "ENDELSE") {
                attachedChild = ifElseNode->removeCurrentNodeFromIRTree();
                nodeDeleted = true;
            }
        }
        else if (dynamic_cast<LoopNode*>(currentNode) != nullptr) {
            LoopNode* loopNode = dynamic_cast<LoopNode*>(currentNode);
            if (dynamic_cast<EndBlockNode*>(loopNode->getChildren()[0]) != nullptr && dynamic_cast<EndBlockNode*>(loopNode->getChildren()[0])->getText() == "ENDBODY") {
                attachedChild = loopNode->removeCurrentNodeFromIRTree();
                nodeDeleted = true;
            }
        }
        else if (dynamic_cast<LoopCondNode*>(currentNode) != nullptr) {
            LoopCondNode* loopCondNode = dynamic_cast<LoopCondNode*>(currentNode);
            if (dynamic_cast<EndBlockNode*>(loopCondNode->getChildren()[2]) != nullptr && dynamic_cast<EndBlockNode*>(loopCondNode->getChildren()[2])->getText() == "ENDBODY") {
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
            for (BaseNode* child : currentNode->getChildren()) {
                nodesToVisit.push(child);
            }
        }
         //continue dfs

        atLeastOneNodeDeleted = atLeastOneNodeDeleted || nodeDeleted;
    }

    return atLeastOneNodeDeleted;
}

