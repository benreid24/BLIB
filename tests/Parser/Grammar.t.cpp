#include <BLIB/Parser/Grammar.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
TEST(Grammar, ReductionLookup) {
    const Node::Type x = 1;
    const Node::Type y = 2;
    const Node::Type C = 3;
    const Node::Type B = 4;
    const Node::Type A = 5;

    Grammar grammar;
    grammar.setStart(A);
    grammar.addRule(C, x);      // C -> x
    grammar.addRule(B, y);      // B -> y
    grammar.addRule(A, {B, C}); // A -> B C
    ASSERT_TRUE(grammar.compile());

    Grammar::Reduction r = grammar.reductionLookup({x});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, C);

    r = grammar.reductionLookup({y});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, B);

    r = grammar.reductionLookup({B});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, Node::None);

    r = grammar.reductionLookup({C});
    EXPECT_EQ(r.reductionsPossible, 0);
    EXPECT_EQ(r.result, Node::None);

    r = grammar.reductionLookup({x, y});
    EXPECT_EQ(r.reductionsPossible, 0);
    EXPECT_EQ(r.result, Node::None);

    r = grammar.reductionLookup({B, C});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, A);
}

TEST(Grammar, Disambiguate) {
    const Node::Type C = 1;
    const Node::Type B = 2;
    const Node::Type A = 3;

    Grammar grammar;
    grammar.addRule(A, {B, C}); // A -> B C
    grammar.addRule(A, B);      // A -> B
    ASSERT_TRUE(grammar.compile());

    Grammar::Reduction r = grammar.reductionLookup({B});
    EXPECT_EQ(r.reductionsPossible, 2);
    EXPECT_EQ(r.result, A);

    r = grammar.reductionLookup({B, C});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, A);
}

TEST(Grammar, Combine) {
    const Node::Type B = 1;
    const Node::Type A = 2;

    Grammar grammar;
    grammar.addRule(A, {A, B}); // A -> A B
    grammar.addRule(A, B);      // A -> B
    ASSERT_TRUE(grammar.compile());

    Grammar::Reduction r = grammar.reductionLookup({B});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, A);

    r = grammar.reductionLookup({A, B});
    EXPECT_EQ(r.reductionsPossible, 1);
    EXPECT_EQ(r.result, A);
}

TEST(Grammar, Ambiguous) {
    const Node::Type C = 1;
    const Node::Type B = 2;
    const Node::Type A = 3;

    Grammar grammar;
    grammar.addRule(A, B); // A -> B
    grammar.addRule(C, B); // C -> B
    ASSERT_FALSE(grammar.compile());
}

} // namespace unittest
} // namespace parser
} // namespace bl