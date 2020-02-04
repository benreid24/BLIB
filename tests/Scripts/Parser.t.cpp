#include <Scripts/Parser.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace scripts
{
TEST(Parser, GrammarCompile) {
    parser::Grammar grammar = Parser::getGrammar();
    EXPECT_TRUE(grammar.compile());
}

TEST(Parser, Tokens) {
    using G                                                             = Parser::Grammar;
    const std::vector<std::pair<parser::Node::Type, std::string>> tests = {
        std::make_pair(G::NumLit, "16.76"),
        std::make_pair(G::NumLit, "0.05"),
        std::make_pair(G::NumLit, "16"),
        std::make_pair(G::StringLit, "\"hello world\""),
        std::make_pair(G::StringLit, "\"test\\\" escape\""),
        std::make_pair(G::Def, "def"),
        std::make_pair(G::If, "if"),
        std::make_pair(G::While, "while"),
        std::make_pair(G::Return, "return"),
        std::make_pair(G::And, "and"),
        std::make_pair(G::Or, "or"),
        std::make_pair(G::Not, "not"),
        std::make_pair(G::LParen, "("),
        std::make_pair(G::RParen, ")"),
        std::make_pair(G::LBrkt, "["),
        std::make_pair(G::RBrkt, "]"),
        std::make_pair(G::LBrc, "{"),
        std::make_pair(G::RBrc, "}"),
        std::make_pair(G::Assign, "="),
        std::make_pair(G::Eq, "=="),
        std::make_pair(G::Ne, "!="),
        std::make_pair(G::Gt, ">"),
        std::make_pair(G::Ge, ">="),
        std::make_pair(G::Lt, "<"),
        std::make_pair(G::Le, "<="),
        std::make_pair(G::Amp, "&"),
        std::make_pair(G::Dot, "."),
        std::make_pair(G::Plus, "+"),
        std::make_pair(G::Minus, "-"),
        std::make_pair(G::Mult, "*"),
        std::make_pair(G::Div, "/"),
        std::make_pair(G::Hat, "^"),
        std::make_pair(G::Comma, ","),
        std::make_pair(G::Term, ";"),
        std::make_pair(G::Id, "varname"),
        std::make_pair(G::Id, "varname09"),
        std::make_pair(G::Id, "varname5id"),
        std::make_pair(G::Id, "Varname"),
        std::make_pair(G::Id, "ifName"),
        std::make_pair(G::Id, "nameif"),
        std::make_pair(G::Id, "pleasewhilework"),
    };

    for (const auto& test : tests) {
        const parser::Tokenizer& t = Parser::getTokenizer();
        parser::Stream s(test.second);
        const std::vector<parser::Node::Ptr> tks = t.tokenize(s);
        ASSERT_EQ(tks.size(), 1) << "Expect 1 token: " + test.second;
        EXPECT_EQ(tks[0]->type, test.first)
            << "Expect " << test.first << " got " << tks[0]->type << ": " << test.second;
    }
}

} // namespace scripts
} // namespace bl