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
    static constexpr Node::Type Asign         = 5;
    static constexpr Node::Type Value         = 6;
    static constexpr Node::Type Num           = 7;
    static constexpr Node::Type Op            = 8;
    static constexpr Node::Type Term          = 9;

    /**
     * S             -> StatementList
     * StatementList -> StatementList Statement
     * StatementList -> Statement //TODO - figure out this
     * Statement     -> Id Asign Value Term
     * Value         -> Id
     * Value         -> Num
     * Value         -> Value Op Value
     */

    ParserFixture()
    : tokenizer(WhitespaceSkipper::create()) {
        // Terminals
        tokenizer.addTokenType(Term, ";");
        tokenizer.addTokenType(Op, "[\\+\\-]");
        tokenizer.addTokenType(Num, "[0-9]+[\\.[0-9]+]?");
        tokenizer.addTokenType(Asign, "=");
        tokenizer.addTokenType(Id, "[a-zA-Z]+[a-ZA-Z0-9]*");

        // Non-terminals
        grammar.addRule(Value, {Value, Op, Value});
        grammar.addRule(Value, Num);
        grammar.addRule(Value, Id);
        grammar.addRule(Statement, {Id, Asign, Value, Term});
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
    //
}

} // namespace unittest
} // namespace bl