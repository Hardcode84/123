#ifndef STREAM_HPP
#define STREAM_HPP

#include <utility>

namespace util
{

namespace detail
{
template<typename Stream, typename Delim>
class DelimitedStream final
{
    Stream& mStream;
    const Delim mDelim;
    bool mWritten = false;
public:
    template<typename DelimT>
    DelimitedStream(Stream& stream, DelimT&& delim):
        mStream(stream), mDelim(std::forward<DelimT>(delim)) {}

    template<typename T>
    DelimitedStream<Stream, Delim>& operator<<(T&& val)
    {
        if(mWritten)
        {
            mStream << mDelim;
        }
        else
        {
            mWritten = true;
        }
        mStream << std::forward<T>(val);
        return *this;
    }
};

}

template<typename Stream, typename Delim>
detail::DelimitedStream<Stream, Delim> delimitedStream(Stream& stream, Delim&& delim)
{
    return detail::DelimitedStream<Stream, Delim>(stream, std::forward<Delim>(delim));
}

}

#endif // STREAM_HPP
