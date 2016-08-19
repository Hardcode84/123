#include "node.hpp"

#include <cassert>

#include "util/utils.hpp"
#include "util/enforce.hpp"

namespace jit
{

void Node::Connect(NodePtr inNode, int inPort, NodePtr outNode, int outPort)
{
    ENFORCE(inPort  >= 0);
    ENFORCE(outPort >= 0);
    ENFORCE(!util::contains(outNode->mInputs, outPort));
    //exception safe
    auto& outPorts = inNode->mOutputs[inPort];
    util::ensureAppend(outPorts);
    outNode->mInputs.insert(std::make_pair(outPort, inNode));
    outPorts.push_back(outNode);
}

bool Node::HasInputs() const
{
    return !mInputs.empty();
}

bool Node::HasOutputs() const
{
    for(auto& out: mOutputs)
    {
        if(!out.second.empty())
        {
            return true;
        }
    }
    return false;
}

}
