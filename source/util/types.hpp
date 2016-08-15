#ifndef TYPES_HPP
#define TYPES_HPP

#include <limits>

#include "util/llvm_warn_push.hpp"
#include "llvm/ADT/SmallVector.h"
#include "util/llvm_warn_pop.hpp"

#include "gsl/gsl.h"

namespace llvm
{
class Type;
}

namespace util
{
template <typename T, unsigned N>
using small_vector = llvm::SmallVector<T, N>;

struct dimension
{
    int begin = 0;
    int size = 0;
};

using MSize = small_vector<int, 3>;
using MDimensions = small_vector<dimension, 3>;

struct Port
{
    MSize size;
    gsl::not_null<llvm::Type*> type;
};
}

#endif // TYPES_HPP
