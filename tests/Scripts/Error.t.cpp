#include <BLIB/Scripts/Error.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace scripts
{
namespace unittest
{
TEST(Error, Message) {
    const std::string msg = "this is an error";
    Error error(msg);
    EXPECT_EQ(error.message(), msg);
}

TEST(Error, Source) {
    const std::string msg = "this is an error";
    parser::Node::Ptr node(new parser::Node());
    node->sourceLine             = 5;
    node->sourceColumn           = 10;
    const std::string stacktrace = "Line 5 position 10: " + msg;

    Error error(msg, node);
    EXPECT_EQ(error.message(), stacktrace);
    EXPECT_EQ(error.stacktrace(), stacktrace);
}

TEST(Error, Stack) {
    const std::string msg = "this is an error";
    parser::Node::Ptr node(new parser::Node());
    node->sourceLine             = 5;
    node->sourceColumn           = 10;
    const std::string stacktrace = "Line 5 position 10: " + msg;

    Error error(msg, node);
    EXPECT_EQ(error.message(), stacktrace);
    EXPECT_EQ(error.stacktrace(), stacktrace);

    const std::string msg2 = "top level error";
    parser::Node::Ptr node2(new parser::Node());
    node2->sourceColumn           = 25;
    node2->sourceLine             = 9;
    const std::string stacktrace2 = "Line 9 position 25: " + msg2 + "\n    " + stacktrace;
    Error error2(msg2, node2, error);
    EXPECT_EQ(error2.message(), "Line 9 position 25: " + msg2);
    EXPECT_EQ(error2.stacktrace(), stacktrace2);
}

} // namespace unittest
} // namespace scripts
} // namespace bl