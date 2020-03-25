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
    parser::Grammar grammar     = Parser::getGrammar();
    parser::Tokenizer tokenizer = Parser::getTokenizer();
    bl::Parser parser(grammar, tokenizer);
    EXPECT_TRUE(parser.valid());
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

using T = Parser::Grammar;
struct ParseTest {
    const Parser::Grammar result;
    const std::string data;
    const bool pass;

    ParseTest(Parser::Grammar g, const std::string& d, bool p)
    : result(g)
    , data(d)
    , pass(p) {}
};
class ScriptParserTest : public ::testing::TestWithParam<ParseTest> {};

TEST_P(ScriptParserTest, Value) {
    const parser::Tokenizer& t = Parser::getTokenizer();
    parser::Grammar grammar    = Parser::getGrammar();
    grammar.setStart(GetParam().result);

    const bl::Parser p(grammar, t);
    ASSERT_TRUE(p.valid());
    EXPECT_EQ(p.parse(GetParam().data).get() != nullptr, GetParam().pass) << GetParam().data;
}

INSTANTIATE_TEST_SUITE_P(
    ScriptParserValue, ScriptParserTest,
    ::testing::Values(
        ParseTest(T::ValueList, "variable", true), ParseTest(T::ValueList, "5.5", true),
        ParseTest(T::ValueList, "17 * 5", true), ParseTest(T::ValueList, "var/9", true),
        ParseTest(T::ValueList, "smth + 5", true), ParseTest(T::ValueList, "6-3", true),
        ParseTest(T::ValueList, "5^2", true), ParseTest(T::ValueList, "3*3^2", true),
        ParseTest(T::ValueList, "5/1", true), ParseTest(T::ValueList, "5+3^2 * 5", true),
        ParseTest(T::ValueList, "3 / (5+3^5)", true),
        ParseTest(T::ValueList, "function(5, variable)", true),
        ParseTest(T::ValueList, "array[5]", true), ParseTest(T::ValueList, "[]", true),
        ParseTest(T::ValueList, "[5, 6, [1, 2]]", true),
        ParseTest(T::ValueList, "[5, 6]", true), ParseTest(T::ValueList, "\"string\"", true),
        ParseTest(T::ValueList, "5 == 6", true), ParseTest(T::ValueList, "6 >= 5", true),
        ParseTest(T::ValueList, "0 <= 2", true), ParseTest(T::ValueList, "10 < 3", true),
        ParseTest(T::ValueList, "5 != 89", true),
        ParseTest(T::ValueList, "this and that", true),
        ParseTest(T::ValueList, "me or you", true),
        ParseTest(T::ValueList, "not variable", true),
        ParseTest(T::Program, "var = 5 + 3;", true),
        ParseTest(T::Program, "var = [5 + 3, \"hello\", func(wow)];", true),
        ParseTest(T::Program, "return 5^(function(5, \"hello\"));", true),
        ParseTest(T::Program, "def func(arg1, arg2) { return arg1+arg2; } func(3.5, 7^2);",
                  true),
        ParseTest(
            T::Program,
            "if (var == 5 or 3*3 > 2^3) { this.array[5^2].nested = func(1, \"2\", 3.5);}",
            true),
        ParseTest(T::Program,
                  "while (var == 5 or 3*3 > 2^3 and not something) { this.array[5^2].nested = "
                  "func(1, \"2\", 3.5);}",
                  true),
        ParseTest(T::Program,
                  "if (not true) { exit(1); } elif (1 == 5) { print(\"oh no\");}  else { "
                  "return 0; }",
                  true)));

TEST(ScriptParser, FullTree) {
    using G = Parser::Grammar;

    const std::string script = "if (true) { return 5; }";
    parser::Node::Ptr root   = Parser::parse(script);
    ASSERT_NE(root.get(), nullptr);

    ASSERT_EQ(root->type, G::Program);
    ASSERT_EQ(root->children.size(), 1);
    ASSERT_EQ(root->children[0]->type, G::StmtList);

    root = root->children[0];
    ASSERT_EQ(root->children.size(), 1);
    ASSERT_EQ(root->children[0]->type, G::Statement);

    root = root->children[0];
    ASSERT_EQ(root->children.size(), 1);
    ASSERT_EQ(root->children[0]->type, G::Conditional);

    root = root->children[0];
    ASSERT_EQ(root->children.size(), 1);
    ASSERT_EQ(root->children[0]->type, G::ElifChain);

    root = root->children[0];
    ASSERT_EQ(root->children.size(), 1);
    ASSERT_EQ(root->children[0]->type, G::IfBlock);

    root = root->children[0];
    ASSERT_EQ(root->children.size(), 2);
    ASSERT_EQ(root->children[0]->type, G::IfHead);
    ASSERT_EQ(root->children[1]->type, G::StmtBlock);

    parser::Node::Ptr head = root->children[0];
    parser::Node::Ptr body = root->children[1];

    ASSERT_EQ(head->children.size(), 2);
    ASSERT_EQ(head->children[0]->type, G::If);
    ASSERT_EQ(head->children[1]->type, G::PGroup);

    head = head->children[1];
    ASSERT_EQ(head->children.size(), 3);
    ASSERT_EQ(head->children[0]->type, G::LParen);
    ASSERT_EQ(head->children[1]->type, G::Value);
    ASSERT_EQ(head->children[2]->type, G::RParen);

    head = head->children[1];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::OrGrp);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::AndGrp);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::Negation);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::Cmp);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::Sum);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::Product);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::Exp);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::TValue);

    head = head->children[0];
    ASSERT_EQ(head->children.size(), 1);
    ASSERT_EQ(head->children[0]->type, G::True);

    ASSERT_EQ(body->children.size(), 3);
    ASSERT_EQ(body->children[0]->type, G::LBrc);
    ASSERT_EQ(body->children[1]->type, G::StmtList);
    ASSERT_EQ(body->children[2]->type, G::RBrc);

    body = body->children[1];
    ASSERT_EQ(body->children.size(), 1);
    ASSERT_EQ(body->children[0]->type, G::Statement);

    body = body->children[0];
    ASSERT_EQ(body->children.size(), 3);
    ASSERT_EQ(body->children[0]->type, G::Return);
    ASSERT_EQ(body->children[1]->type, G::Value);
    ASSERT_EQ(body->children[2]->type, G::Term);
}

} // namespace unittest
} // namespace scripts
} // namespace bl