#pragma once

#include "BasicBlock.h"
#include "AnalysisTools.h"
#include <vector>
#include <algorithm>
#include <iostream>

// Base template for all data flow analyses
template<typename Domain> //Domain is the type of the data flow set, e.g. set of expressions or variables
class BaseDataFlowAnalysis {

    public:
    
        ///TODO: maybe change implementation to flowgraph nodes instead of basic blocks??
        // //returns the dataflow sets for each instruction node in the program
        std::unordered_map<BaseNode*, Domain> getNodeDataFlowSets();

        //returns the basic blocks used for the dataflow analysis
        std::vector<BasicBlock*> getBasicBlocksUsed();

        //print the dataflow sets for each basic block
        virtual void printBlockDataFlowSets();

    protected:

        //definitions:
        // enum class MeetOperation {
        //     INTERSECTION,
        //     UNION
        // };

        enum class AnalysisDirection {
            FORWARD,
            BACKWARD
        };

        //member variables
        // MeetOperation meetOperation;
        AnalysisDirection analysisDirection;  //could these be private to the base class?
        BasicBlock* entryBasicBlock;
        std::vector<BasicBlock*> basicBlocks;
        std::vector<Domain> blockDataFlowSets;    //stores the data flow sets computed for each basic block. Allows easy computation of the meet operation (e.g. outset for FORWARD, inset for BACKWARD)
        std::unordered_map<BaseNode*, Domain> nodeDataFlowSets;    //stores the data flow sets computed for each instruction node. Allows easy usage in optimisation passes (e.g. inset for FORWARD, outset for BACKWARD)
        Domain initialSet;    //the initial set of the dataflow set
       

        ////////////////////////////////////////////////////////////////
        /// NEED TO CALL THESE IN THE CONCRETE CLASS CONSTRUCTOR
        ////////////////////////////////////////////////////////////////

        //base class constructor and setInitialSet() method, to be called in the concrete class constructor
        BaseDataFlowAnalysis(BasicBlock* entryBasicBlock, AnalysisDirection analysisDirection);
        void setInitialSet(Domain initialSet);

        //computes the dataflow sets for program flowgraph
        void computeDataFlowSets();


        ////////////////////////////////////////////////////////////////
        /// TO BE IMPLEMENTED BY CONCRETE ANALYSES SUBCLASSES
        ////////////////////////////////////////////////////////////////

        //returns a new set of the dataflow set after processing the current instruction node
        virtual Domain transferFunction(BaseNode* instructionNode, const Domain& inputSet) = 0; 

        //returns the meet of two dataflow sets, i.e. implement the intersection or union over the Domain type
        virtual Domain meetOperation(const Domain& set1, const Domain& set2) = 0;




    private:

        //computes the dataflow sets for each basic block
        Domain basicBlockComputeDataFlowSet(BasicBlock* basicBlock);

        // //returns the dataflow sets for each basic block
        // std::vector<Domain> getBlockDataFlowSets();

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DEFINE THE TEMPLATE CLASS - need to be done in the header file for the template class to be instantiated in the concrete classes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Domain>
BaseDataFlowAnalysis<Domain>::BaseDataFlowAnalysis(BasicBlock *entryBasicBlock, AnalysisDirection analysisDirection) : entryBasicBlock(entryBasicBlock), analysisDirection(analysisDirection)
{
    // set the basic blocks used for the analysis
    this->basicBlocks = AnalysisTools::getBasicBlocks(entryBasicBlock);
    if (analysisDirection == AnalysisDirection::BACKWARD)
    {
        std::reverse(basicBlocks.begin(), basicBlocks.end());
    }

    // set the node dataflow sets
    ///NOTE: have to do this in the concrete classes because computeDataFlowSets() is virtual in the base class
    // computeDataFlowSets();
}

template <typename Domain>
void BaseDataFlowAnalysis<Domain>::setInitialSet(Domain initialSet){
    this->initialSet = initialSet;
}

template <typename Domain>
std::vector<BasicBlock *> BaseDataFlowAnalysis<Domain>::getBasicBlocksUsed()
{
    return basicBlocks;
}

template <typename Domain>
std::unordered_map<BaseNode *, Domain> BaseDataFlowAnalysis<Domain>::getNodeDataFlowSets()
{
    return nodeDataFlowSets;
}

// template <typename Domain>
// std::vector<Domain> BaseDataFlowAnalysis<Domain>::getBlockDataFlowSets()
// {
//     return blockDataFlowSets;
// }

template <typename Domain>
void BaseDataFlowAnalysis<Domain>::computeDataFlowSets()
{

    // initialise the dataflow sets for each basic block
    blockDataFlowSets = std::vector<Domain>(basicBlocks.size(), initialSet);

    // iterate until no more changes
    bool changed = true;
    while (changed)
    {
        changed = false;
        for (int i = 0; i < basicBlocks.size(); i++)
        { // start from 0, process all basic blocks
            Domain new_dataflow_set = basicBlockComputeDataFlowSet(basicBlocks[i]);
            if (new_dataflow_set != blockDataFlowSets[i])
            { // check if the reaches set has changed, if so, update the reaches set and set changed to true
                blockDataFlowSets[i] = new_dataflow_set;
                changed = true;
            }
        }
    }
}

template <typename Domain>
Domain BaseDataFlowAnalysis<Domain>::basicBlockComputeDataFlowSet(BasicBlock *basicBlock)
{

    Domain dataflowSet = initialSet;    //create the dataflow set for traversing through this basic block

    if (analysisDirection == AnalysisDirection::FORWARD)
    {
        std::vector<BasicBlock *> predecessors = basicBlock->get_predecessors();
        if (predecessors.empty()) {
            dataflowSet = {};
        }else {
            for (BasicBlock* predecessor : predecessors) {
                //get the dataflow set for the predecessor
                int predecessorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), predecessor) - basicBlocks.begin();
                Domain predecessorDataflowSet = blockDataFlowSets[predecessorIndex];
                //meet the dataflow set with the predecessor dataflow set
                dataflowSet = meetOperation(dataflowSet, predecessorDataflowSet);
            }
        }
        //now we have the in-dataflow set of the basic block
        //store it in the nodeDataFlowSets map for the instruction before we process that instruction with the transfer function

        const std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
        for (auto it = instructions.begin(); it != instructions.end(); ++it) {
            BaseNode* instruction = *it;

            //store the IN-dataflow set for the instruction in the nodeDataFlowSets map
            nodeDataFlowSets[instruction] = dataflowSet;

            //process the instruction with the transfer function
            dataflowSet = transferFunction(instruction, dataflowSet);
        }

        //OUT-dataflow set of the basic block is now stored in the dataflowSet variable
    }
    else if (analysisDirection == AnalysisDirection::BACKWARD)
    {
        std::vector<BasicBlock*> successors = basicBlock->get_successors();
        for (BasicBlock* successor : successors) {
            //get the dataflow set for the successor
            int successorIndex = std::find(basicBlocks.begin(), basicBlocks.end(), successor) - basicBlocks.begin();
            Domain successorDataflowSet = blockDataFlowSets[successorIndex];
            //meet the dataflow set with the successor dataflow set
            dataflowSet = meetOperation(dataflowSet, successorDataflowSet);
        }

        //now we have the out-dataflow set of the basic block
        //store it in the nodeDataFlowSets map for the instruction before we process that instruction with the transfer function

        const std::list<BaseNode*>& instructions = basicBlock->get_instructions_reference();
        for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {   //iterate backwards
            BaseNode* instruction = *it;

            //store the OUT-dataflow set for the instruction in the nodeDataFlowSets map
            nodeDataFlowSets[instruction] = dataflowSet;

            //process the instruction with the transfer function
            dataflowSet = transferFunction(instruction, dataflowSet);
        }

        //IN-dataflow set of the basic block is now stored in the dataflowSet variable
    }

    return dataflowSet;
}

template <typename Domain>
void BaseDataFlowAnalysis<Domain>::printBlockDataFlowSets()
{
    for (int i = 0; i < basicBlocks.size(); i++)
    {
        std::cout << "-------------------------\nBasic block: " << basicBlocks[i]->getText() << std::endl;
        for (const auto &dataflowSet : blockDataFlowSets[i])
        { // get the dataflow set for the basic block
            std::cout << "Dataflow set: " << dataflowSet << std::endl;
        }
    }
    std::cout << "-------------------------\n"
              << std::endl;
}

