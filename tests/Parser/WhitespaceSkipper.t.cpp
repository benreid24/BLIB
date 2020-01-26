#include <BLIB/Parser/WhitespaceSkipper.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
TEST(WhitespaceSkipper, BasicSkip) {
    const std::string data = " \n\t  \r  d";
    Stream stream(data);
    WhitespaceSkipper skipper;
    skipper.skip(stream);
    EXPECT_EQ('d', stream.get());
}

TEST(WhitespaceSkipper, DoubleSkip) {
    const std::string data = "  t\n  \t  w";
    Stream stream(data);
    WhitespaceSkipper skipper;
    skipper.skip(stream);
    EXPECT_EQ('t', stream.get());
    skipper.skip(stream);
    EXPECT_EQ('w', stream.get());
}

TEST(WhitespaceSkipper, MultipleSkip) {
    const std::string data = "  \nt";
    Stream stream(data);
    WhitespaceSkipper skipper;
    skipper.skip(stream);
    EXPECT_EQ(stream.peek(), 't');
    skipper.skip(stream);
    EXPECT_EQ('t', stream.get());
}

} // namespace unittest
} // namespace parser
} // namespace bl