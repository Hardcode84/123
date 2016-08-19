#include "graph_compiler.hpp"

#include "jit/node.hpp"
#include "jit/graph.hpp"

#include "util/utils.hpp"
#include "util/enforce.hpp"

#include <cassert>
#include <unordered_set>

namespace jit
{
namespace compiler
{

void CheckCycles(const Graph& graph)
{
    ENFORCE(graph.GetNodesCount() > 0);
    std::unordered_set<NodePtr> visited;
    graph.EnumNodes([&](NodePtr node)
    {
        node->VisitAll<true>([&](int port, NodePtr node)
        {
            (void)port;
            ENFORCE(!util::contains(visited, node));
            visited.insert(node);
        });
        return false;
    });

    ENFORCE(visited.size() == graph.GetNodesCount());
}

SortedNodes SortNodes(const Graph& graph)
{
    assert(graph.GetNodesCount() > 0);
    // We assume there are no cycles in graph at this point
    SortedNodes ret;
    std::unordered_set<NodePtr> visited;
    graph.EnumNodes([&](NodePtr node)
    {
        node->VisitInputs<true>([&](int port, NodePtr node)
        {
            (void)port;
            if(!util::contains(visited, node))
            {
                ret.sortedNodes.push_back(node);
                if(!node->HasInputs())
                {
                    ret.srcNodes.push_back(node);
                }
                if(!node->HasOutputs())
                {
                    ret.dstNodes.push_back(node);
                }
                visited.insert(node);
            }
        });
    });
    return ret;
}

}

}
