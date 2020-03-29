#include <BLIB/Parser/Grammar.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
using ::testing::ElementsAre;
namespace
{
const int S = 1;
const int A = 2;
const int B = 3;
const int C = 4;
const int x = 5;
const int y = 6;

Grammar get() {
    Grammar grammar;
    grammar.addTerminal(x);
    grammar.addTerminal(y);
    grammar.addNonTerminal(S);
    grammar.addNonTerminal(A);
    grammar.addNonTerminal(B);
    grammar.addNonTerminal(C);
    return grammar;
}

void addProd(Grammar& grammar) {
    grammar.addRule(S, A);
    grammar.addRule(A, {B, C});
    grammar.addRule(A, {A, A});
    grammar.addRule(B, x);
    grammar.addRule(C, y);
    grammar.setStart(S);
}

} // namespace

TEST(Grammar, SymbolTypes) {
    Grammar grammar = get();

    EXPECT_TRUE(grammar.terminal(x));
    EXPECT_FALSE(grammar.nonterminal(x));
    EXPECT_TRUE(grammar.terminal(y));
    EXPECT_FALSE(grammar.nonterminal(y));

    EXPECT_TRUE(grammar.nonterminal(A));
    EXPECT_FALSE(grammar.terminal(A));
    EXPECT_TRUE(grammar.nonterminal(B));
    EXPECT_FALSE(grammar.terminal(B));
    EXPECT_TRUE(grammar.nonterminal(C));
    EXPECT_FALSE(grammar.terminal(C));

    EXPECT_FALSE(grammar.nonterminal(-1));
    EXPECT_FALSE(grammar.terminal(-1));
}

TEST(Grammar, Productions) {
    Grammar grammar = get();

    EXPECT_TRUE(grammar.addRule(S, A));
    EXPECT_TRUE(grammar.addRule(A, {B, C}));
    EXPECT_TRUE(grammar.addRule(A, {A, A}));
    EXPECT_TRUE(grammar.addRule(B, x));
    EXPECT_TRUE(grammar.addRule(C, y));

    EXPECT_FALSE(grammar.addRule(S, -1));
    EXPECT_FALSE(grammar.addRule(-1, S));
    EXPECT_FALSE(grammar.addRule(-1, -1));
}

TEST(Grammar, Follow) {
    Grammar grammar = get();
    addProd(grammar);

    EXPECT_THAT(grammar.followSet(S), ElementsAre(Node::EOI));
    EXPECT_THAT(grammar.followSet(A), ElementsAre(x, Node::EOI));
    EXPECT_THAT(grammar.followSet(B), ElementsAre(y));
    EXPECT_THAT(grammar.followSet(C), ElementsAre(x, Node::EOI));
    EXPECT_THAT(grammar.followSet(x), ElementsAre(y));
    EXPECT_THAT(grammar.followSet(y), ElementsAre(x, Node::EOI));
}

TEST(Grammar, Closure) {
    Grammar grammar = get();
    addProd(grammar);

    Grammar::Production r0 = {S, {A}};
    Grammar::Production r1 = {A, {B, C}};
    Grammar::Production r2 = {A, {A, A}};
    Grammar::Production r3 = {B, {x}};
    Grammar::Production r4 = {C, {y}};

    Grammar::Item i0(r0, 0);
    Grammar::Item i1(r1, 0);
    Grammar::Item i2(r3, 0);
    Grammar::Item i3(r2, 0);

    Grammar::ItemSet s0 = grammar.closure(i0);
    EXPECT_THAT(s0.items(), ElementsAre(i0, i1, i2, i3));
}

} // namespace unittest
} // namespace parser
} // namespace bl