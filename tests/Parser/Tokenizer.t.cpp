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
    EXPECT_EQ(nodes[0]->type, id);
    EXPECT_EQ(nodes[0]->data, "hello");
}

TEST(Tokenizer, BasicRegex) {
    const Node::Type id    = 1;
    const std::string data = "alpha5";
    Stream stream(data);

    const std::string regex = "[a-z]*[0-9]";
    Tokenizer tokenizer(WhitespaceSkipper::create());
    tokenizer.addTokenType(id, regex);
    const std::vector<Node::Ptr> nodes = tokenizer.tokenize(stream);

    ASSERT_EQ(nodes.size(), 1);
    EXPECT_EQ(nodes[0]->type, id);
    EXPECT_EQ(nodes[0]->data, "alpha5");
}

TEST(Tokenizer, RegexSubgroup) {
    const Node::Type id    = 1;
    const std::string data = "\"string lit\"";
    Stream stream(data);

    const std::string regex = "\"(.*)\"";
    Tokenizer tokenizer(WhitespaceSkipper::create());
    tokenizer.addTokenType(id, regex);
    tokenizer.addSkipperToggleChar('"');
    const std::vector<Node::Ptr> nodes = tokenizer.tokenize(stream);

    ASSERT_EQ(nodes.size(), 1);
    EXPECT_EQ(nodes[0]->type, id);
    EXPECT_EQ(nodes[0]->data, "string lit");
}

TEST(Tokenizer, EscapeSequence) {
    const Node::Type id    = 1;
    const std::string data = "hello\\n";
    Stream stream(data);

    const std::string regex = "hello\\n";
    Tokenizer tokenizer(nullptr);
    tokenizer.addTokenType(id, regex);

    std::vector<Node::Ptr> nodes = tokenizer.tokenize(stream);
    ASSERT_EQ(nodes.size(), 0);

    tokenizer.addEscapeSequence("\\n", '\n');
    Stream stream2(data);
    nodes = tokenizer.tokenize(stream2);
    ASSERT_EQ(nodes.size(), 1);
    EXPECT_EQ(nodes[0]->type, id);
    EXPECT_EQ(nodes[0]->data, "hello\n");
}

TEST(Tokenizer, MultipleTokens) {
    const std::string data = "5.5 + variable";
    Stream stream(data);
    Tokenizer tokenizer(WhitespaceSkipper::create());

    const std::string numRegex = "[0-9]+[\\.[0-9]*]?";
    const Node::Type numId     = 1;
    tokenizer.addTokenType(numId, numRegex);

    const Node::Type opId = 2;
    tokenizer.addTokenType(opId, "\\+");

    const Node::Type varId     = 3;
    const std::string varRegex = "[a-zA-Z]+[a-zA-Z0-9]*";
    tokenizer.addTokenType(varId, varRegex);

    const std::vector<Node::Ptr> nodes = tokenizer.tokenize(stream);

    ASSERT_EQ(nodes.size(), 3);
    EXPECT_EQ(nodes[0]->type, numId);
    EXPECT_EQ(nodes[0]->data, "5.5");
    EXPECT_EQ(nodes[1]->type, opId);
    EXPECT_EQ(nodes[1]->data, "+");
    EXPECT_EQ(nodes[2]->type, varId);
    EXPECT_EQ(nodes[2]->data, "variable");
}

} // namespace unittest

} // namespace parser

} // namespace bl