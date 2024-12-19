#include "DotTreeTools.h"

std::string DotTreeTools::irTreeToDot(BaseNode *root)
{

    std::stringstream dot;
    dot << "digraph Tree {\n";

    int nodeCount = 0;
    std::unordered_map<BaseNode *, int> nodeToIndex; // map to store node and its index - used in dot file

    std::stack<BaseNode *> stack;
    stack.push(root);

    while (!stack.empty())
    {
        BaseNode *currentNode = stack.top();
        stack.pop();

        if (nodeToIndex.find(currentNode) == nodeToIndex.end())
        {
            nodeToIndex[currentNode] = nodeCount++; // add node to map if not already in it

            // get text representation of node
            std::string nodeText = currentNode->getText();
            nodeText = std::regex_replace(nodeText, std::regex("\""), "\\\""); // escape quotes for printing
            dot << "node" << nodeToIndex[currentNode] << " [label=\"" << nodeText << "\"];\n";
        }

        // add children to stack
        for (BaseNode *child : currentNode->getChildren())
        {
            // create child node if not in map
            if (nodeToIndex.find(child) == nodeToIndex.end())
            {
                nodeToIndex[child] = nodeCount++;
                std::string childText = child->getText();
                childText = std::regex_replace(childText, std::regex("\""), "\\\"");
                dot << "node" << nodeToIndex[child] << " [label=\"" << childText << "\"];\n";
            }

            // add edge to dot file
            dot << "node" << nodeToIndex[currentNode] << " -> node" << nodeToIndex[child] << ";\n";

            // add child to stack
            stack.push(child);
        }
    }

    dot << "}\n";
    return dot.str();
}



std::string DotTreeTools::parseTreeToDot(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames)
{
  std::stringstream dot; // stream to store dot file
  dot << "digraph Tree {\n";
  int nodeCount = 0;
  std::unordered_map<antlr4::tree::ParseTree *, int> nodeToIndex; // map to store node and its index - used in dot file

  // Use a stack to perform DFS
  std::stack<antlr4::tree::ParseTree *> stack;
  stack.push(root);

  while (!stack.empty())
  {
    antlr4::tree::ParseTree *node = stack.top();
    stack.pop();

    // if node is not in the map (iterated to end of tree and did not find it), add it to the map and the dot file
    if (nodeToIndex.find(node) == nodeToIndex.end())
    {
      nodeToIndex[node] = nodeCount++;

      // gets text representation of node in parse tree and escapes whitespaces
      // replace " with \"
      std::string nodeText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(node, ruleNames), false);
      nodeText = std::regex_replace(nodeText, std::regex("\""), "\\\"");

      dot << "node" << nodeToIndex[node] << " [label=\"" << nodeText << "\"];\n";
    }

    // add children nodes to stack
    for (auto child : node->children)
    {

      if (nodeToIndex.find(child) == nodeToIndex.end())
      { // create node for child if not in map

        nodeToIndex[child] = nodeCount++;
        std::string childText = antlrcpp::escapeWhitespace(antlr4::tree::Trees::getNodeText(child, ruleNames), false);
        childText = std::regex_replace(childText, std::regex("\""), "\\\"");
        dot << "node" << nodeToIndex[child] << " [label=\"" << childText << "\"];\n";
      }

      // add edge to dot file
      dot << "node" << nodeToIndex[node] << " -> node" << nodeToIndex[child] << ";\n";

      // add child to stack
      stack.push(child);
    }
  }

  dot << "}\n";
  return dot.str();
}

///NOTE: this is a graph, not a tree, so need to maintain list of seen basic blocks
std::string DotTreeTools::flowgraphToDot(BasicBlock* root) {

   std::stringstream dot;
    dot << "digraph Tree {\n";

    int nodeCount = 0;
    std::unordered_map<BasicBlock *, int> nodeToIndex; // map to store node and its index - used in dot file

    std::stack<BasicBlock *> stack;
    stack.push(root);

    std::vector<BasicBlock*> seenBasicBlocks;
   
    while (!stack.empty())
    {
        BasicBlock *currentNode = stack.top();
        stack.pop();

        if (nodeToIndex.find(currentNode) == nodeToIndex.end())
        {
            nodeToIndex[currentNode] = nodeCount++; // add node to map if not already in it

            // get text representation of node
            std::string nodeText = currentNode->getText();
            dot << "node" << nodeToIndex[currentNode] << " [label=\"" << nodeText << "\"];\n";
        }

        // add children to stack
        for (BasicBlock *child : currentNode->get_successors())
        {
           
            // create child node if not in map
            if (nodeToIndex.find(child) == nodeToIndex.end())
            {
                nodeToIndex[child] = nodeCount++;
                std::string childText = child->getText();
                dot << "node" << nodeToIndex[child] << " [label=\"" << childText << "\"];\n";
            }

            // add edge to dot file - even if the child has already been seen, this edge is still needed
            dot << "node" << nodeToIndex[currentNode] << " -> node" << nodeToIndex[child] << ";\n";

            // add child to stack if it has not been visited yet
            if (std::find(seenBasicBlocks.begin(), seenBasicBlocks.end(), child) == seenBasicBlocks.end()) {
                stack.push(child);
                seenBasicBlocks.push_back(child);     //now you have seen this basic block
            }
             

        }
    }

    dot << "}\n";
    return dot.str();

}