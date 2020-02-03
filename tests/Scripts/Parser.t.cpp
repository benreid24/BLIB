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

}
}