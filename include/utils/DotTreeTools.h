#pragma once
#include "BaseNode.h"
#include <stack>
#include <regex>
#include "antlr4-runtime.h"

class DotTreeTools
{
public:
    static std::string irTreeToDot(BaseNode *root);

    static std::string parseTreeToDot(antlr4::tree::ParseTree *root, const std::vector<std::string> &ruleNames);

private:
    DotTreeTools() = default;    // no constructor, class just contains static methods

};