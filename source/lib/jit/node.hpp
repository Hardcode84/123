#ifndef NODE_HPP
#define NODE_HPP

#include "gsl/gsl.h"
#include "util/types.hpp"
#include "util/utils.hpp"

#include <vector>
#include <unordered_map>

namespace jit
{
namespace detail
{
    class NodeInterface;
}

class Graph;
class GraphCompiler;
class Node;

using NodePtr = gsl::not_null<Node*>;

class Node
{
public:
    friend class jit::Graph;
    friend class jit::GraphCompiler;
    static void Connect(NodePtr inNode, int inPort, NodePtr outNode, int outPort);

    template<bool Recursive = false, typename F>
    bool VisitInputs(F&& func) const;

    template<bool Recursive = false, typename F>
    bool VisitOutputs(F&& func) const;

    template<bool Recursive = false, typename F>
    bool VisitAll(F&& func) const;

    bool HasInputs() const;
    bool HasOutputs() const;

protected:
    virtual gsl::not_null<detail::NodeInterface*> GetNodeInterface() = 0;

private:
    virtual ~Node() = default; //owned by graph
    std::unordered_map<int, NodePtr> mInputs;
    std::unordered_map<int, std::vector<NodePtr>> mOutputs;
};

template<bool Recursive, typename F>
bool Node::VisitInputs(F&& func) const
{
    for(auto& in: mInputs)
    {
        const int port = in.first;
        auto node = in.second;
        if(Recursive)
        {
            if(!node->VisitInputs(std::forward<F>(func)))
            {
                return false;
            }
        }

        if(!util::checkReturnIfCan<true>(std::forward<F>(func),port, node))
        {
            return false;
        }
    }
    return true;
}

template<bool Recursive, typename F>
bool Node::VisitOutputs(F&& func) const
{
    for(auto& out: mOutputs)
    {
        for(auto& outNode: out.second)
        {
            const int port = out.first;
            if(!util::checkReturnIfCan<true>(std::forward<F>(func), port, outNode))
            {
                return false;
            }

            if(Recursive)
            {
                if(!outNode->VisitOutputs(std::forward<F>(func)))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

template<bool Recursive, typename F>
bool Node::VisitAll(F&& func) const
{
    return VisitInputs<Recursive>(std::forward<F>(func)) && VisitOutputs<Recursive>(std::forward<F>(func));
}

}

#endif // NODE_HPP
