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
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->deref().value().getAsFloat(), 10.f);
}

TEST(SymbolTable, Stack1) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value("hello"));
    table.pushFrame();
    table.set("second", Value(10));
    ASSERT_TRUE(table.exists("var"));
    ASSERT_TRUE(table.exists("second"));
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->deref().value().getAsInt(), 10);
    table.popFrame();
    ASSERT_TRUE(table.exists("var"));
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
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
    ASSERT_FALSE(table.exists("second"));
    ASSERT_TRUE(table.exists("deep"));
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
    EXPECT_EQ(table.get("deep")->deref().value().getAsString(), "world");
    table.popFrame();
    ASSERT_TRUE(table.exists("second"));
    EXPECT_EQ(table.get("second")->deref().value().getAsFloat(), 10.f);
    table.popFrame();
    ASSERT_TRUE(table.exists("var"));
    ASSERT_FALSE(table.exists("second"));
    ASSERT_FALSE(table.exists("deep"));
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
}

TEST(SymbolTable, ExtraPop) {
    SymbolTable table;

    EXPECT_FALSE(table.exists("var"));
    table.set("var", Value("hello"));
    table.pushFrame();
    table.set("second", Value(10));
    ASSERT_TRUE(table.exists("var"));
    ASSERT_TRUE(table.exists("second"));
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
    EXPECT_EQ(table.get("second")->deref().value().getAsInt(), 10);
    table.popFrame();
    EXPECT_THROW(table.popFrame(), Error);
    ASSERT_TRUE(table.exists("var"));
    EXPECT_EQ(table.get("var")->deref().value().getAsString(), "hello");
    EXPECT_FALSE(table.exists("second"));
}

} // namespace unittest
} // namespace script
} // namespace bl
