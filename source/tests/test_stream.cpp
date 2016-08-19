#include "gtest/gtest.h"

#include "util/stream.hpp"

#include <sstream>

TEST(StreamTest, Delimited)
{
    {
        std::stringstream ss;
        util::delimitedStream(ss,", ") << "foo";
        EXPECT_EQ("foo", ss.str());
    }
    {
        std::stringstream ss;
        util::delimitedStream(ss,std::string(", ")) << "foo";
        EXPECT_EQ("foo", ss.str());
    }
    {
        std::stringstream ss;
        util::delimitedStream(ss,", ") << "foo" << "bar";
        EXPECT_EQ("foo, bar", ss.str());
    }
    {
        std::stringstream ss;
        util::delimitedStream(ss,std::string(", ")) << "foo" << "bar";
        EXPECT_EQ("foo, bar", ss.str());
    }
}
