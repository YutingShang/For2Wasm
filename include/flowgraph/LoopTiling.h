#pragma once

#include "BaseNode.h"
#include "LoopCondNode.h"
#include <map>

//doesn't need the flowgraph, just analyses the IR tree to find perfectly nested loops (LoopCondNode)and tiles them

class LoopTiling {

    public:

        LoopTiling(std::shared_ptr<EntryNode> entryNode, int nextLoopLabelNumber, int nextIfLabelNumber, int nextTempVariableCount, std::unordered_map<std::string, std::string> irDatatypeMap);

        //perform the loop tiling transformation, returns whether the loop tiling changed the program
        bool runLoopTiling();

        //get the updated irDatatypeMap after the loop tiling transformation
        std::unordered_map<std::string, std::string> getUpdatedIRDatatypeMap() const;


    private:
        std::shared_ptr<EntryNode> entryNode;
        int loopLabelNumber;
        int ifLabelNumber;
        int tempVariableCount;
        const std::string TILE_SIZE = "4";

        std::unordered_map<std::string, std::string> irDatatypeMap;

        //returns a new temp variable name _t, and also adds the datatype to the irDatatypeMap
        std::string getNewTempVariableName(std::string IRdatatype);

        struct LoopComparator {
            bool operator()(const std::shared_ptr<LoopCondNode>& a, const std::shared_ptr<LoopCondNode>& b) const {
                return a->getLabelNumber() < b->getLabelNumber();
            }
        };

        //get the perfectly nested loops, which are the ones we want to tile 
        std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>, LoopComparator> getPerfectlyNestedLoops();

        //map of an OUTER loopCondNode to a vector of its perfectly nested INNER loopCondNodes (if there are multiple, they are nested; if there are none, its just an outer loop - can still do loop stripmining)
        std::map<std::shared_ptr<LoopCondNode>, std::vector<std::shared_ptr<LoopCondNode>>, LoopComparator> loopCondNodesToTile;

        //e.g. for 'do i=1, 10' then return all the extra code for the 'do ii=1, 10, tile' etc code
        //also modifies the original loopCondNode to have the correct bounds
        std::shared_ptr<LoopCondNode> getNewLoopCondNodeForTiledLoop(std::shared_ptr<LoopCondNode> loopCondNode);

        //for the new tiled loop created by getNewLoopCondNodeForTiledLoop, return the ENDBODY block node (after the ENDIF)
        std::shared_ptr<EndBlockNode> getLoopCondNodeEndBodyNode(std::shared_ptr<LoopCondNode> loopCondNode);

        //nest the inner loopCondNode inside the outer loopCondNode, sandwich before the ENDBODY of the outer loopCondNode
        void nestLoopCondNodes(std::shared_ptr<LoopCondNode> outerLoopCondNode, std::shared_ptr<LoopCondNode> innerLoopCondNode);

        //keep track of which variables have already been added to the entry node (declarations), so we don't add them again
        std::vector<std::string> tilingVariablesAdded;

        //returns a pair of the top and bottom nodes of the copied arithmetic condition (copied from the original condition node)
        std::pair<std::shared_ptr<ArithOpNode>, std::shared_ptr<ArithOpNode>> getCopyOfArithCondition(std::shared_ptr<ArithOpNode> startArithOpNode);

        //get the original upper bound of the loopCondNode (i.e. goes all the way down and finds the actual upper bound, e.g. (GT _t2 i B), then returns B)
        std::string getOriginalUpperBound(std::shared_ptr<LoopCondNode> loopCondNode);

        //modify the upper bound of the loopCondNode, by replacing the original loop variable with the new loop variable
        void modifyUpperBoundCondNode(std::shared_ptr<LoopCondNode> loopCondNode, std::string newLoopVariable);

};