#include <BLIB/Parser/CommentSkipper.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
TEST(CommentSkipper, NoComment) {
    const std::string data = "var = 4 + 5;";
    Stream stream(data);
    CommentSkipper skipper("//", "/*", "*/");
    skipper.skip(stream);
    EXPECT_EQ(stream.peek(), 'v');
}

TEST(CommentSkipper, LineComment) {
    const std::string data = "// this is a comment\nvar = 4 + 5;";
    Stream stream(data);
    CommentSkipper skipper("//", "/*", "*/");
    skipper.skip(stream);
    EXPECT_EQ(stream.peek(), 'v');
}

TEST(CommentSkipper, BlockComment) {
    const std::string data = "/* how about some\nmore comment\n*/ var = 4 + 5;";
    Stream stream(data);
    CommentSkipper skipper("//", "/*", "*/");
    skipper.skip(stream);
    EXPECT_EQ(stream.peek(), 'v');
}

TEST(CommentSkipper, EmptyBlock) {
    const std::string data = "/**/ var = 4 + 5;";
    Stream stream(data);
    CommentSkipper skipper("//", "/*", "*/");
    skipper.skip(stream);
    EXPECT_EQ(stream.peek(), 'v');
}

TEST(CommentSkipper, Both) {
    const std::string data = "/*wogh\nman*/ // comments\nvar = 4 + 5;";
    Stream stream(data);
    CommentSkipper skipper("//", "/*", "*/");
    skipper.skip(stream);
    EXPECT_EQ(stream.peek(), 'v');
}

} // namespace unittest
} // namespace parser
} // namespace bl