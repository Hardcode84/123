#include "gtest/gtest.h"

#include "util/utils.hpp"

#include <memory>

TEST(UtilsTest, CheckReturnIfCan)
{
    EXPECT_TRUE(util::checkReturnIfCan<true>([](int i)
    {
        EXPECT_EQ(41, i);
    }, 41));

    EXPECT_FALSE(util::checkReturnIfCan<false>([](int i)
    {
        EXPECT_EQ(42, i);
    }, 42));

    EXPECT_TRUE(util::checkReturnIfCan<true>([](int i)
    {
        EXPECT_EQ(43, i);
        return true;
    }, 43));

    EXPECT_FALSE(util::checkReturnIfCan<true>([](int i)
    {
        EXPECT_EQ(44, i);
        return false;
    }, 44));

    EXPECT_TRUE(util::checkReturnIfCan<false>([](int i)
    {
        EXPECT_EQ(45, i);
        return true;
    }, 45));

    EXPECT_FALSE(util::checkReturnIfCan<false>([](int i)
    {
        EXPECT_EQ(46, i);
        return false;
    }, 46));

    // Check arg forwarding
    EXPECT_TRUE(util::checkReturnIfCan<true>([](std::unique_ptr<int> p)
    {
        EXPECT_NE(nullptr, p);
    }, std::make_unique<int>(5)));

    EXPECT_TRUE(util::checkReturnIfCan<true>([](std::unique_ptr<int> p)
    {
        EXPECT_NE(nullptr, p);
        return true;
    }, std::make_unique<int>(5)));
}
