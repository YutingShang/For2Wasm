#pragma once

#include "BasicBlock.h"
#include "PlaceholderNode.h"


// this class is used for when you want to insert a new basic block into the flowgraph (but it doesn't connect nicely to the IR tree)
// you provide a method to actually insert the instruction node into the correct position in the IR tree

// it contains a placeholder instruction node so dataflow analysis can be done on the basic block
// the placeholder instruction node is removed when an instruction node is actually inserted into the IR tree

///NOTE: currently only supports inserting a single SIMPLENODE!!! instruction node into the IR tree?
///but might also work if inserting linear sequence of simpleNode instructions into the IR tree

//insertable basic block wil always contain a PLACEHOLDER instruction node!!
class InsertableBasicBlock : public BasicBlock {

    public:

        //use strategy pattern so users can define their own method to insert the instruction node into the IR tree
        //ONLY WORKS FOR SIMPLENODES!!!
        class NodeInsertionStrategy {   //inner class

            public:

                //constructor and destructor
                NodeInsertionStrategy() = default;
                virtual ~NodeInsertionStrategy() = default;

                //either uses the first insertion strategy or the default subsequent insertion strategy
                virtual void insertNodeIntoIRTree(std::shared_ptr<SimpleNode> node){
                    if (isFirstInsertion){
                        firstInsertionStrategy(node);
                        lastInsertedNode = node;
                        isFirstInsertion = false;
                    }
                    else{
                        subsequentInsertionStrategy(node);
                    }
                }

                //first insertion strategy (e.g. could involve more complex logic) - must be overridden by user
                virtual void firstInsertionStrategy(std::shared_ptr<SimpleNode> node)=0;

                //default insertion strategy (e.g. insert another instruction) - must be overridden by user
                virtual void subsequentInsertionStrategy(std::shared_ptr<SimpleNode> node){
                    //insert after the last inserted node
                    if (lastInsertedNode.expired()){
                        throw std::runtime_error("No last inserted node found");
                    }
                    lastInsertedNode.lock()->insertSandwichChild(node);
                    lastInsertedNode = node;
                }

            protected:
                bool isFirstInsertion = true;

                std::weak_ptr<SimpleNode> lastInsertedNode;  //for subsequent insertions, just insert after the last inserted node
        };

        //constructor to take in the strategy
        InsertableBasicBlock(std::unique_ptr<NodeInsertionStrategy> strategy);

        //virtual destructor
        virtual ~InsertableBasicBlock() = default;

        //uses the strategy when needed to insert the node into the IR tree
        //does not delete the placeholder instruction node after insertion
        void executeNodeIRInsertion(std::shared_ptr<SimpleNode> nodeToInsert);

        //to the instructions list, add the placeholder instruction node
        //call in constructor
        void addPlaceholderInstructionNode();

    private:
        std::unique_ptr<NodeInsertionStrategy> insertionStrategy;    //overall insertion strategy, includes first and subsequent insertion strategies - but typically user only needs to override the first insertion strategy

        //the insertable basic block owns the placeholder node
        std::shared_ptr<PlaceholderNode> placeholderNode;

        

};