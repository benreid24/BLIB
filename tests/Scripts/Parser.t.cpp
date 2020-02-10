#include <Scripts/Parser.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace scripts
{
namespace unittest
{
using NodeString = std::pair<parser::Node::Type, std::string>;
using G          = Parser::Grammar;

TEST(ScriptParser, GrammarCompile) {
    parser::Grammar grammar = Parser::getGrammar();
    EXPECT_TRUE(grammar.compile());
}

class ScriptParserTokenTest : public ::testing::TestWithParam<NodeString> {};

TEST_P(ScriptParserTokenTest, Tokens) {
    const std::vector<NodeString> tests = {
        std::make_pair(G::NumLit, "16.76"), std::make_pair(G::NumLit, "0.05"),
        std::make_pair(G::NumLit, "16"),    std::make_pair(G::StringLit, "\"hello world\""),
        std::make_pair(G::Def, "def"),      std::make_pair(G::If, "if"),
        std::make_pair(G::While, "while"),  std::make_pair(G::Return, "return"),
        std::make_pair(G::And, "and"),      std::make_pair(G::Or, "or"),
        std::make_pair(G::Not, "not"),      std::make_pair(G::LParen, "("),
        std::make_pair(G::RParen, ")"),     std::make_pair(G::LBrkt, "["),
        std::make_pair(G::RBrkt, "]"),      std::make_pair(G::LBrc, "{"),
        std::make_pair(G::RBrc, "}"),       std::make_pair(G::Assign, "="),
        std::make_pair(G::Eq, "=="),        std::make_pair(G::Ne, "!="),
        std::make_pair(G::Gt, ">"),         std::make_pair(G::Ge, ">="),
        std::make_pair(G::Lt, "<"),         std::make_pair(G::Le, "<="),
        std::make_pair(G::Amp, "&"),        std::make_pair(G::Dot, "."),
        std::make_pair(G::Plus, "+"),       std::make_pair(G::Minus, "-"),
        std::make_pair(G::Mult, "*"),       std::make_pair(G::Div, "/"),
        std::make_pair(G::Hat, "^"),        std::make_pair(G::Comma, ","),
        std::make_pair(G::Term, ";"),       std::make_pair(G::Id, "varname"),
        std::make_pair(G::Id, "varname09"), std::make_pair(G::Id, "varname5id"),
        std::make_pair(G::Id, "Varname"),   std::make_pair(G::Id, "ifName"),
        std::make_pair(G::Id, "nameif"),    std::make_pair(G::Id, "pleasewhilework"),
    };

    const auto& test           = GetParam();
    const parser::Tokenizer& t = Parser::getTokenizer();
    parser::Stream s(test.second);
    const std::vector<parser::Node::Ptr> tks = t.tokenize(s);
    ASSERT_EQ(tks.size(), 1) << "Expect 1 token: " + test.second;
    EXPECT_EQ(tks[0]->type, test.first)
        << "Expect " << test.first << " got " << tks[0]->type << ": " << test.second;
}

INSTANTIATE_TEST_SUITE_P(
    ScriptParserTokens, ScriptParserTokenTest,
    ::testing::Values(
        NodeString(G::NumLit, "16.76"), NodeString(G::NumLit, "0.05"),
        NodeString(G::NumLit, "16"), NodeString(G::StringLit, "\"hello world\""),
        NodeString(G::Def, "def"), NodeString(G::If, "if"), NodeString(G::While, "while"),
        NodeString(G::Return, "return"), NodeString(G::And, "and"), NodeString(G::Or, "or"),
        NodeString(G::Not, "not"), NodeString(G::LParen, "("), NodeString(G::RParen, ")"),
        NodeString(G::LBrkt, "["), NodeString(G::RBrkt, "]"), NodeString(G::LBrc, "{"),
        NodeString(G::RBrc, "}"), NodeString(G::Assign, "="), NodeString(G::Eq, "=="),
        NodeString(G::Ne, "!="), NodeString(G::Gt, ">"), NodeString(G::Ge, ">="),
        NodeString(G::Lt, "<"), NodeString(G::Le, "<="), NodeString(G::Amp, "&"),
        NodeString(G::Dot, "."), NodeString(G::Plus, "+"), NodeString(G::Minus, "-"),
        NodeString(G::Mult, "*"), NodeString(G::Div, "/"), NodeString(G::Hat, "^"),
        NodeString(G::Comma, ","), NodeString(G::Term, ";"), NodeString(G::Id, "varname"),
        NodeString(G::Id, "varname09"), NodeString(G::Id, "varname5id"),
        NodeString(G::Id, "Varname"), NodeString(G::Id, "ifName"), NodeString(G::Id, "nameif"),
        NodeString(G::Id, "pleasewhilework")));

TEST(ScriptParser, StringLiteral) {
    const parser::Tokenizer& t = Parser::getTokenizer();
    parser::Stream slit("\"hello\"");
    std::vector<parser::Node::Ptr> tks = t.tokenize(slit);
    ASSERT_EQ(tks.size(), 1);
    EXPECT_EQ(tks[0]->type, G::StringLit);
    EXPECT_EQ(tks[0]->data, "hello");
}

class ScriptParserValueTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ScriptParserValueTest, Value) {
    const parser::Tokenizer& t = Parser::getTokenizer();
    parser::Grammar grammar    = Parser::getGrammar();

    ASSERT_TRUE(grammar.compile());
    grammar.setStart(Parser::Grammar::ValueList);

    const bl::Parser p(grammar, t);
    EXPECT_NE(p.parse(GetParam()).get(), nullptr) << GetParam();
}

INSTANTIATE_TEST_SUITE_P(ScriptParserValue, ScriptParserValueTest,
                         ::testing::Values("variable", "5.5", "17 * 5", "var/9", "smth + 5",
                                           "6-3", "5^2", "3*3^2", "5/1", "5+3^2 * 5",
                                           "3 / (5+3^5)", "function(5, variable)", "array[5]",
                                           "{}", "{5, 6}", "\"string\"", "5 == 6", "6 >= 5",
                                           "0 <= 2", "10 < 3", "5 != 89", "this and that",
                                           "me or you", "not variable"));
} // namespace unittest
} // namespace scripts
} // namespace bl