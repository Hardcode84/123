#ifndef GRAPH_H
#define GRAPH_H

#include "node.hpp"

#include "util/utils.hpp"

#include <vector>
#include <memory>
#include <utility>

namespace llvm
{
class Context;
class Module;
class Function;
class Type;
}

namespace jit
{

class Graph final
{
public:
    Graph();

    template<typename Checker, typename Mapper, typename Processor>
    inline NodePtr CreateNode(Checker&& checker, Mapper&& mapper, Processor&& processor);

    template<typename F>
    void EnumNodes(F&& func) const;

    size_t GetNodesCount() const;

private:
    struct NodeDeleter final
    {
        void operator()(Node* node) const
        {
            delete node;
        }
    };

    std::vector<std::unique_ptr<Node, NodeDeleter>> mNodes;
};

namespace detail
{
class NodeInterface
{
public:
    ~NodeInterface() = default;

    struct SetupParams
    {
        llvm::Context& context;
    };

    struct SetupInputParams
    {
        util::small_vector<util::Port, 3> ports;
    };

    struct SetupOutputParams
    {
        util::small_vector<util::Port, 3> ports;
    };

    virtual void Setup(const SetupInputParams& inparams, SetupOutputParams& outparams) = 0;

    struct AssembleFunctionParams
    {
        llvm::Module& module;
    };

    virtual gsl::not_null<llvm::Function*> AssembleFunction(const AssembleFunctionParams& params) = 0;
};
}

template<typename Checker, typename Mapper, typename Processor>
NodePtr Graph::CreateNode(Checker&& checker, Mapper&& mapper, Processor&& processor)
{
    class NodeImpl : public Node
    {
        const std::remove_cv<Checker> mChecker;
        const std::remove_cv<Mapper>::type mMapper;
        const std::remove_cv<Processor>::type mProcessor;
    public:
        NodeImpl(Checker&& checker, Mapper&& mapper, Processor&& processor):
            mChecker(std::move(checker)), mMapper(std::move(mapper)), mProcessor(std::move(processor)) {}
    };

    mNodes.emplace_back(std::make_unique<NodeImpl>(
                            std::forward<Checker>(checker),
                            std::forward<Mapper>(mapper),
                            std::forward<Processor>(processor)));
    return mNodes.back();
}

template<typename F>
void Graph::EnumNodes(F&& func) const
{
    for(auto& node: mNodes)
    {
        if(!util::checkReturnIfCan<true>(std::forward<F>(func),NodePtr(node.get())))
        {
            return;
        }
    }
}

}

#endif // GRAPH_H
