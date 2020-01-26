#include <BLIB/Parser/Parser.hpp>

#include <BLIB/Parser.hpp>
#include <gtest/gtest.h>

namespace bl
{
using namespace parser;
namespace unittest
{
class ParserFixture : public ::testing::Test {
public:
    static constexpr Node::Type S             = 1;
    static constexpr Node::Type StatementList = 2;
    static constexpr Node::Type Statement     = 3;
    static constexpr Node::Type Id            = 4;
    static constexpr Node::Type Assign        = 5;
    static constexpr Node::Type Value         = 6;
    static constexpr Node::Type Num           = 7;
    static constexpr Node::Type Op            = 8;
    static constexpr Node::Type Term          = 9;

    /**
     * S             -> StatementList
     * StatementList -> StatementList Statement
     * StatementList -> Statement
     * Statement     -> Id Assign Value Term
     * Value         -> Id
     * Value         -> Num
     * Value         -> Value Op Value
     */

    ParserFixture()
    : tokenizer(WhitespaceSkipper::create()) {
        // Terminals
        tokenizer.addTokenType(Term, ";");
        tokenizer.addTokenType(Op, "[\\+\\-]");
        tokenizer.addTokenType(Num, "[0-9]+");
        tokenizer.addTokenType(Assign, "=");
        tokenizer.addTokenType(Id, "[a-zA-Z]+[a-ZA-Z0-9]*");

        // Non-terminals
        grammar.addRule(Value, {Value, Op, Value});
        grammar.addRule(Value, Num);
        grammar.addRule(Value, Id);
        grammar.addRule(Statement, {Id, Assign, Value, Term});
        grammar.addRule(StatementList, Statement);
        grammar.addRule(StatementList, {StatementList, Statement});
        grammar.addRule(S, StatementList);
        grammar.setStart(S);
    }

    Tokenizer tokenizer;
    Grammar grammar;

protected:
    virtual void SetUp() override {}
    virtual void TearDown() override {}
};

TEST_F(ParserFixture, BasicParse) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_NE(root.get(), nullptr);
}

TEST_F(ParserFixture, LongParse) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 + 5; var2= 7+ 5 ;\n\twogh = 6-5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_NE(root.get(), nullptr);
}

TEST_F(ParserFixture, ProperTree) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_NE(root.get(), nullptr);

    EXPECT_EQ(root->type, S);
    ASSERT_EQ(root->children.size(), 1);

    root = root->children[0];
    EXPECT_EQ(root->type, StatementList);
    ASSERT_EQ(root->children.size(), 1);

    root = root->children[0];
    EXPECT_EQ(root->type, Statement);
    ASSERT_EQ(root->children.size(), 4);
    EXPECT_EQ(root->children[0]->type, Id);
    EXPECT_EQ(root->children[1]->type, Assign);
    EXPECT_EQ(root->children[2]->type, Value);
    EXPECT_EQ(root->children[3]->type, Term);
    EXPECT_EQ(root->children[0]->data, "var");

    root = root->children[2];
    ASSERT_EQ(root->children.size(), 3);
    EXPECT_EQ(root->children[0]->type, Value);
    EXPECT_EQ(root->children[1]->type, Op);
    EXPECT_EQ(root->children[2]->type, Value);
    EXPECT_EQ(root->children[1]->data, "+");

    ASSERT_EQ(root->children[0]->children.size(), 1);
    EXPECT_EQ(root->children[0]->children[0]->type, Num);
    EXPECT_EQ(root->children[0]->children[0]->data, "3");

    ASSERT_EQ(root->children[2]->children.size(), 1);
    EXPECT_EQ(root->children[2]->children[0]->type, Num);
    EXPECT_EQ(root->children[2]->children[0]->data, "5");
}

TEST_F(ParserFixture, SyntaxFail1) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 + + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail2) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 = 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail3) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var  3 + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail4) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 + 5; 5";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail5) {
    ASSERT_TRUE(grammar.compile());
    Parser parser(grammar, tokenizer);
    const std::string data = "var = 3 + 5; var";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

} // namespace unittest
} // namespace bl