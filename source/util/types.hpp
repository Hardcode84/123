#ifndef TYPES_HPP
#define TYPES_HPP

#include <limits>
#include <cassert>
#include <cstdint>
#include <utility>
#include <type_traits>

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
template<typename>
struct func_type; // undefined

template<typename R, typename... Args>
struct func_type<R(Args...)>
{
    typedef R (*type)(Args...);
};

template <typename T, unsigned N>
using small_vector = llvm::SmallVector<T, N>;

struct dimension final
{
    int begin = 0;
    int size = 0;
};

struct span_dimension final
{
   int size = 0;
   int stride = 0;
};

using MSize = small_vector<int, 3>;
using MOffset = small_vector<int, 3>;
using MDimensions = small_vector<dimension, 3>;
using MSpans = small_vector<span_dimension, 3>;

struct Port final
{
    MSize size;
    gsl::not_null<llvm::Type*> type;
};



}

#endif // TYPES_HPP
