#ifndef GRAPHCOMPILER_HPP
#define GRAPHCOMPILER_HPP

#include "jit/node.hpp"

#include <vector>

namespace jit
{
class Graph;

namespace compiler
{
void CheckCycles(const Graph& graph);

struct SortedNodes final
{
    std::vector<NodePtr> srcNodes;
    std::vector<NodePtr> dstNodes;
    std::vector<NodePtr> sortedNodes;
};

SortedNodes SortNodes(const Graph& graph);

};

}
#endif // GRAPHCOMPILER_HPP
