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
class Node
{
public:
    friend class jit::Graph;
    static void Connect(gsl::not_null<Node*> inNode, int inPort, gsl::not_null<Node*> outNode, int outPort);

    template<typename F>
    void VisitInputs(F&& func) const;

    template<typename F>
    void VisitOutputs(F&& func) const;

    bool HasInputs() const;
    bool HasOutputs() const;

protected:
    virtual gsl::not_null<detail::NodeInterface*> GetNodeInterface() = 0;

private:
    virtual ~Node() = default; //owned by graph
    std::unordered_map<int, gsl::not_null<Node*>> mInputs;
    std::unordered_map<int, std::vector<gsl::not_null<Node*>>> mOutputs;
};

template<typename F>
void Node::VisitInputs(F&& func) const
{
    for(auto& in: mInputs)
    {
        if(!checkReturnIfCan<true>(std::forward<F>(func),in.first, in.second))
        {
            return;
        }
    }
}

template<typename F>
void Node::VisitOutputs(F&& func) const
{
    for(auto& out: node->mOutPorts)
    {
        for(auto& outNode: out.second)
        {
            if(!checkReturnIfCan<true>(std::forward<F>(func),out.first, outNode))
            {
                return;
            }
        }
    }
}

}

#endif // NODE_HPP
