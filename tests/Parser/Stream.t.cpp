#include <BLIB/Parser/Stream.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
TEST(Stream, Invalidate) {
    const std::string data = "hello i don't matter";
    Stream stream(data);
    EXPECT_TRUE(stream.valid());
    stream.invalidate();
    EXPECT_FALSE(stream.valid());
}

TEST(Stream, InvalidateEOF) {
    const std::string data = "uh oh don't read too far";
    Stream stream(data);
    EXPECT_TRUE(stream.valid());
    for (unsigned int i = 0; i < data.size() + 1; ++i) stream.get();
    EXPECT_FALSE(stream.valid());
}

TEST(Stream, Content) {
    const std::string data = "is the data valid?";
    Stream stream(data);
    EXPECT_TRUE(stream.valid());
    for (unsigned int i = 0; i < data.size(); ++i) { EXPECT_EQ(data[i], stream.get()); }
    EXPECT_TRUE(stream.valid());
}

TEST(Stream, Location) {
    const std::string data = "123\n123\n123";
    const int lines[]      = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2};
    const int cols[]       = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2};
    Stream stream(data);
    EXPECT_TRUE(stream.valid());
    for (unsigned int i = 0; i < data.size(); ++i) {
        EXPECT_EQ(stream.currentLine(), lines[i]);
        EXPECT_EQ(stream.currentColumn(), cols[i]);
        stream.get();
    }
}

} // namespace unittest
} // namespace parser
} // namespace bl