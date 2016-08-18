#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdlib>
#include <utility>
#include <type_traits>

namespace util
{

template<typename T>
inline void ensureCapacity(T& container, size_t capacity)
{
    size_t newCapacity = container.capacity();
    if(newCapacity >= capacity)
    {
        return;
    }
    //TODO: use log
    while(newCapacity < capacity) newCapacity *= 2;
    container.reserve(newCapacity);
}

template<typename T>
inline void ensureAppend(T& container)
{
    ensureCapacity(container, container.size() + 1);
}

template<typename C, typename T>
inline bool contains(const C& container, const T& value)
{
    return container.end() != container.find(value);
}

template<typename Stream, typename First, typename... Other>
void streamWrite(Stream& stream, First&& first, Other&& ... other)
{
    stream << first;
    streamWrite(stream, other...);
}

template<typename Stream>
void streamWrite(Stream& stream)
{
    (void)stream;
    //Nothing
}

template<bool Default, typename F, typename... Args>
bool checkReturnIfCan(F&& func, Args&&... args);

namespace detail
{
template<typename>
struct CheckReturnHelper; // undefined

template<>
struct CheckReturnHelper<bool>
{
    using type = std::integral_constant<bool, true>;
};

template<>
struct CheckReturnHelper<void>
{
    using type = std::integral_constant<bool, false>;
};

template<bool Default, typename F, typename... Args>
bool checkReturnIfCanImpl(std::integral_constant<bool, true>,F&& func, Args&&... args)
{
    return F(std::forward<Args>(args));
}

template<bool Default, typename F, typename... Args>
bool checkReturnIfCanImpl(std::integral_constant<bool, false>,F&& func, Args&&... args)
{
    F(std::forward<Args>(args));
    return Default;
}

} // namespace detail

template<bool Default, typename F, typename... Args>
bool checkReturnIfCan(F&& func, Args&&... args)
{
    return detail::checkReturnIfCanImpl<Default, F, Args>(detail::CheckReturnHelper<decltype(F(std::forward<Args>(args)))>::type(),
                                                          std::forward<F>(func),
                                                          std::forward<Args>(args));
}

} // namespace util

#endif // UTILS_HPP
