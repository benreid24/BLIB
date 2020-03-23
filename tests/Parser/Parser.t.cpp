#include <BLIB/Parser/Parser.hpp>

#include <BLIB/Parser.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace parser
{
namespace unittest
{
class TableReader {
public:
    TableReader(const Parser& parser)
    : parser(parser) {}

    struct Action {
        enum Type { Shift, Reduce } type;
        std::optional<parser::Grammar::Production> reduction;

        Action(const Parser::Action& a)
        : type(static_cast<Type>(a.type))
        , reduction(a.reduction) {}
    };

    unsigned int stateCount() const { return parser.table.size(); }
    std::optional<Grammar::ItemSet> getState(unsigned int s) const {
        std::optional<Grammar::ItemSet> r;
        if (s < parser.table.size()) r = parser.table[s].state;
        return r;
    }
    std::optional<std::map<parser::Node::Type, unsigned int>> stateGoto(unsigned int s) const {
        std::optional<std::map<parser::Node::Type, unsigned int>> r;
        if (s < parser.table.size()) r = parser.table[s].gotos;
        return r;
    }
    std::optional<Action> getAction(unsigned int state, Node::Type la) const {
        std::optional<Action> r;
        if (state < parser.table.size()) {
            if (parser.table[state].actions.find(la) != parser.table[state].actions.end()) {
                r = Action(parser.table[state].actions.at(la));
            }
        }
        return r;
    }

private:
    const Parser& parser;
};

TEST(Parser, Table) {
    const Node::Type S = 1;
    const Node::Type A = 2;
    const Node::Type B = 3;
    const Node::Type x = 4;
    const Node::Type y = 5;

    Grammar grammar;
    grammar.addTerminal(x);
    grammar.addTerminal(y);
    grammar.addNonTerminal(S);
    grammar.addNonTerminal(A);
    grammar.addNonTerminal(B);
    ASSERT_TRUE(grammar.addRule(S, {A, B}));
    ASSERT_TRUE(grammar.addRule(A, x));
    ASSERT_TRUE(grammar.addRule(B, y));
    grammar.setStart(S);

    Parser parser(grammar, Tokenizer(WhitespaceSkipper::create()));
    ASSERT_TRUE(parser.valid());
}

} // namespace unittest
} // namespace parser

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
        grammar.addTerminal(Term);
        grammar.addTerminal(Op);
        grammar.addTerminal(Num);
        grammar.addTerminal(Assign);
        grammar.addTerminal(Id);
        grammar.addNonTerminal(Value);
        grammar.addNonTerminal(Statement);
        grammar.addNonTerminal(StatementList);
        grammar.addNonTerminal(S);

        // Terminals
        tokenizer.addTokenType(Term, ";");
        tokenizer.addTokenType(Op, "[\\+\\-]");
        tokenizer.addTokenType(Num, "[0-9]+");
        tokenizer.addTokenType(Assign, "=");
        tokenizer.addTokenType(Id, "[a-zA-Z]+[a-zA-Z0-9]*");

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
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var = 3 + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_NE(root.get(), nullptr);
}

TEST_F(ParserFixture, LongParse) {
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var = 3 + 5; var2= 7+ 5 ;\n\twogh = 6-5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_NE(root.get(), nullptr);
}

TEST_F(ParserFixture, ProperTree) {
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
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
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var = 3 + + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail2) {
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var = 3 = 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail3) {
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var  3 + 5;";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail4) {
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var = 3 + 5; 5";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

TEST_F(ParserFixture, SyntaxFail5) {
    Parser parser(grammar, tokenizer);
    ASSERT_TRUE(parser.valid());
    const std::string data = "var = 3 + 5; var";
    Node::Ptr root         = parser.parse(data);
    ASSERT_EQ(root.get(), nullptr);
}

} // namespace unittest
} // namespace bl