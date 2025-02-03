#pragma once
#include "BaseNode.h"
#include "BasicBlock.h"
#include <stack>
#include <regex>
#include "antlr4-runtime.h"

//uses graphviz to create dot files

class DotTreeTools
{
public:

    DotTreeTools() = delete;    // no constructor, class just contains static methods

    static std::string irTreeToDot(std::shared_ptr<BaseNode> root);

    static std::string parseTreeToDot(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames);

    static std::string flowgraphToDot(BasicBlock* root);

    

};