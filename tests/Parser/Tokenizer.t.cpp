#include <BLIB/Parser/Tokenizer.hpp>
#include <BLIB/Parser/WhitespaceSkipper.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
TEST(Tokenizer, SingleWord) {
    const Node::Type id    = 1;
    const std::string data = "hello";
    Stream stream(data);

    const std::string regex = "hello";
    Tokenizer tokenizer(WhitespaceSkipper::create());
    tokenizer.addTokenType(id, regex);
    const std::vector<Node::Ptr> nodes = tokenizer.tokenize(stream);

    ASSERT_EQ(nodes.size(), 1);
}

} // namespace unittest

} // namespace parser

} // namespace bl