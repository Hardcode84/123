#ifndef NODE_HPP
#define NODE_HPP

#include "gsl/gsl.h"
#include "util/types.hpp"

namespace llvm
{
class Context;
class Module;
class Function;
class Type;
}

namespace jit
{
class Node
{
public:
    virtual ~Node() = default;

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

#endif // NODE_HPP
