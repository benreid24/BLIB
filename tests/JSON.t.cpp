#include <BLIB/Files/JSON.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace json
{
namespace unittest
{
TEST(JSON, BoolValue) {
    Value val(true);
    EXPECT_EQ(Value::TBool, val.getType());
    EXPECT_TRUE(val.getAsBool());
    EXPECT_TRUE(val.getAsBool().value());
    val = false;
    EXPECT_EQ(Value::TBool, val.getType());
    EXPECT_TRUE(val.getAsBool());
    EXPECT_FALSE(val.getAsBool().value());
}

TEST(JSON, NumericalValue) {
    Value val(15.5f);
    EXPECT_EQ(Value::TNumeric, val.getType());
    EXPECT_TRUE(val.getAsNumeric());
    EXPECT_EQ(val.getAsNumeric().value(), 15.5f);
    val = 32.3f;
    EXPECT_EQ(Value::TNumeric, val.getType());
    EXPECT_TRUE(val.getAsNumeric());
    EXPECT_EQ(val.getAsNumeric().value(), 32.3f);
}

TEST(JSON, StringValue) {
    Value val("hello");
    EXPECT_EQ(Value::TString, val.getType());
    EXPECT_TRUE(val.getAsString());
    EXPECT_EQ(val.getAsString().value(), "hello");
    val = "world";
    EXPECT_EQ(Value::TString, val.getType());
    EXPECT_TRUE(val.getAsString());
    EXPECT_EQ(val.getAsString().value(), "world");
}

TEST(JSON, ListValue) {
    List list;
    Value val(list);
    EXPECT_EQ(Value::TList, val.getType());
    EXPECT_TRUE(val.getAsList());
}

TEST(JSON, GroupValue) {
    Group group;
    Value val(group);
    EXPECT_EQ(Value::TGroup, val.getType());
    EXPECT_TRUE(val.getAsGroup());
}

} // namespace unittest
} // namespace json
} // namespace bl