#include <BLIB/Scripts/SymbolTable.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
TEST(SymbolTable, Exists) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value());
    EXPECT_TRUE(table.exists("var"));
}

TEST(SymbolTable, Get) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value("hello"));
    table.set("second", Value(10.f));
    ASSERT_TRUE(table.exists("var"));
    ASSERT_TRUE(table.exists("second"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->value().getAsNum(), 10);
}

TEST(SymbolTable, Stack1) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value("hello"));
    table.pushFrame();
    table.set("second", Value(10.f));
    ASSERT_TRUE(table.exists("var"));
    ASSERT_TRUE(table.exists("second"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->value().getAsNum(), 10.f);
    table.popFrame();
    ASSERT_TRUE(table.exists("var"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
    EXPECT_FALSE(table.exists("second"));
}

TEST(SymbolTable, Stack2) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value("hello"));
    table.pushFrame();
    table.set("second", Value(10.f));
    table.pushFrame();
    table.set("deep", Value("world"));
    ASSERT_TRUE(table.exists("var"));
    ASSERT_TRUE(table.exists("second"));
    ASSERT_TRUE(table.exists("deep"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->value().getAsNum(), 10.f);
    EXPECT_EQ(table.get("deep")->value().getAsString(), "world");
    table.popFrame();
    table.popFrame();
    ASSERT_TRUE(table.exists("var"));
    ASSERT_FALSE(table.exists("second"));
    ASSERT_FALSE(table.exists("deep"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
}

TEST(SymbolTable, ExtraPop) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value("hello"));
    table.pushFrame();
    table.set("second", Value(10.f));
    ASSERT_TRUE(table.exists("var"));
    ASSERT_TRUE(table.exists("second"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->value().getAsNum(), 10);
    table.popFrame();
    table.popFrame();
    table.popFrame();
    ASSERT_TRUE(table.exists("var"));
    EXPECT_EQ(table.get("var")->value().getAsString(), "hello");
    EXPECT_FALSE(table.exists("second"));
}

} // namespace unittest
} // namespace script
} // namespace bl
