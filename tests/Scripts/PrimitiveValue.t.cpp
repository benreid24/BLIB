#include <BLIB/Scripts/Function.hpp>
#include <BLIB/Scripts/PrimitiveValue.hpp>
#include <BLIB/Scripts/Script.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
namespace unittest
{
TEST(PrimitiveValue, Void) {
    PrimitiveValue val;
    EXPECT_EQ(val.getType(), PrimitiveValue::TVoid);
}

TEST(PrimitiveValue, Numeric) {
    PrimitiveValue val;
    EXPECT_EQ(val.getType(), PrimitiveValue::TVoid);
    val = 15.f;
    EXPECT_EQ(val.getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(val.getAsNum(), 15.f);
}

TEST(PrimitiveValue, String) {
    PrimitiveValue val;
    EXPECT_EQ(val.getType(), PrimitiveValue::TVoid);
    val = "hello";
    EXPECT_EQ(val.getType(), PrimitiveValue::TString);
    EXPECT_EQ(val.getAsString(), "hello");
}

TEST(PrimitiveValue, Array) {
    PrimitiveValue val;
    ArrayValue arr(3);
    arr[0] = PrimitiveValue(12.f);
    arr[1] = PrimitiveValue("world");
    arr[2] = PrimitiveValue(1.f);

    EXPECT_EQ(val.getType(), PrimitiveValue::TVoid);
    val = arr;
    EXPECT_EQ(val.getType(), PrimitiveValue::TArray);
    ASSERT_EQ(val.getAsArray().size(), 3);
    EXPECT_EQ(val.getAsArray()[0].value().getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(val.getAsArray()[0].value().getAsNum(), 12.f);
    EXPECT_EQ(val.getAsArray()[1].value().getType(), PrimitiveValue::TString);
    EXPECT_EQ(val.getAsArray()[1].value().getAsString(), "world");
    EXPECT_EQ(val.getAsArray()[2].value().getType(), PrimitiveValue::TNumeric);
    EXPECT_EQ(val.getAsArray()[2].value().getAsNum(), 1.f);
}

} // namespace unittest
} // namespace script
} // namespace bl
