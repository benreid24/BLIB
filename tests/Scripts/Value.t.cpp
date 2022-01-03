#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/Script.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
TEST(Value, Void) {
    Value val;
    EXPECT_EQ(val.getType(), Value::TVoid);
}

TEST(Value, Numeric) {
    Value val;
    EXPECT_EQ(val.getType(), Value::TVoid);
    val = 15.f;
    EXPECT_EQ(val.getType(), Value::TNumeric);
    EXPECT_EQ(val.getAsNum(), 15.f);
}

TEST(Value, String) {
    Value val;
    EXPECT_EQ(val.getType(), Value::TVoid);
    val = "hello";
    EXPECT_EQ(val.getType(), Value::TString);
    EXPECT_EQ(val.getAsString(), "hello");
}

TEST(Value, Array) {
    Value val;
    Value::Array arr(3);
    arr[0] = Value(12.f);
    arr[1] = Value("world");
    arr[2] = Value(1.f);

    EXPECT_EQ(val.getType(), Value::TVoid);
    val = arr;
    EXPECT_EQ(val.getType(), Value::TArray);
    ASSERT_EQ(val.getAsArray().size(), 3);
    EXPECT_EQ(val.getAsArray()[0].getType(), Value::TNumeric);
    EXPECT_EQ(val.getAsArray()[0].getAsNum(), 12.f);
    EXPECT_EQ(val.getAsArray()[1].getType(), Value::TString);
    EXPECT_EQ(val.getAsArray()[1].getAsString(), "world");
    EXPECT_EQ(val.getAsArray()[2].getType(), Value::TNumeric);
    EXPECT_EQ(val.getAsArray()[2].getAsNum(), 1.f);
}

TEST(Value, Ref) {
    Value val;
    Value ref;
    EXPECT_EQ(val.getType(), Value::TVoid);

    val = Value::Ref(&ref, 0);
    ASSERT_EQ(val.getType(), Value::TRef);

    Value& deref = val.deref(0);
    EXPECT_EQ(deref.getType(), Value::TVoid);

    deref = "hello";

    EXPECT_EQ(val.deref(0).getType(), Value::TString);
    EXPECT_EQ(val.deref(0).getAsString(), "hello");
}

} // namespace unittest
} // namespace script
} // namespace bl
