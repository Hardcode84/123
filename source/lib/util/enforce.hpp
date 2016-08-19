#ifndef ENFORCE_HPP
#define ENFORCE_HPP

#include "util/utils.hpp"
#include "util/stream.hpp"

#include <cassert>
#include <stdexcept>
#include <sstream>

namespace util
{
class EnforceError : public std::logic_error
{
public:
    using std::logic_error::logic_error;
};

template<typename Exception = EnforceError, typename... Args>
inline void enforce(bool value, Args&&... args)
{
    if(!value)
    {
        std::stringstream ss;
        streamWrite(delimitedStream(ss, ", "), args...);
        throw Exception(ss.str());
    }
}

template<typename Exception = EnforceError, typename... Args>
inline void enforceMacroHelper(const char* str, bool value, Args&&... args)
{
    assert(nullptr != str);
    if(!value)
    {
        std::stringstream ss;
        ss << "Enforcement \"" << str << "\"failed: ";
        streamWrite(delimitedStream(ss, ", "), args...);
        throw Exception(ss.str());
    }
}

}

#define ENFORCE(...) util::enforceMacroHelper(#__VA_ARGS__, __VA_ARGS__)

#endif // ENFORCE_HPP
