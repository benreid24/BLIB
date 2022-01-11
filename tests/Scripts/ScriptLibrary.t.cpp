#include <BLIB/Scripts.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace script
{
TEST(ScriptLibrary, Print) {
    const std::string input = "x = 5; y = &x; print(true, 5, \"cat\", [1,2], y, abs);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    script.run();
}

TEST(ScriptLibrary, Random) {
    const std::string input = "return random(0, 10);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TNumeric);
    ASSERT_LE(v.value().getAsNum(), 10);
    ASSERT_GE(v.value().getAsNum(), 0);
}

TEST(ScriptLibrary, Run) {
    const std::string input = "return run(\"return 5;\", false);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(v.value().getAsNum(), 5);
}

TEST(ScriptLibrary, RunBgnd) {
    const std::string input = "return run(\"return 5;\", true);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TBool);
    ASSERT_TRUE(v.value().getAsBool());
}

TEST(ScriptLibrary, Exit) {
    const std::string input = "exit(); return 5;";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_FALSE(r.has_value());
}

TEST(ScriptLibrary, Error) {
    const std::string input = "error(\"terminated\"); return 5;";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_FALSE(r.has_value());
}

TEST(ScriptLibrary, Str) {
    const std::string input = "b = true; n = 5; s = \"cat\"; a = [1,2]; r = &n; f = print; "
                              "return str(b)+str(n)+str(s)+str(a)+str(r)+str(f);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TString);
    ASSERT_EQ(r.value().value().getAsString(), "true5cat[1, 2]5<function>");
}

TEST(ScriptLibrary, Num) {
    const std::string input = "return num(\"5\");";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 5);
}

TEST(ScriptLibrary, Sqrt) {
    const std::string input = "return sqrt(16);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 4);
}

TEST(ScriptLibrary, Abs) {
    const std::string input = "return abs(-16);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 16);
}

TEST(ScriptLibrary, Round) {
    const std::string input = "return round(16.1) + round(16.7);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 33);
}

TEST(ScriptLibrary, Floor) {
    const std::string input = "return floor(16.9);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 16);
}

TEST(ScriptLibrary, Ceil) {
    const std::string input = "return ceil(16.1);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 17);
}

TEST(ScriptLibrary, Sin) {
    const std::string input = "return sin(90);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 1);
}

TEST(ScriptLibrary, Cos) {
    const std::string input = "return cos(0);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 1);
}

TEST(ScriptLibrary, Tan) {
    const std::string input = "return tan(0);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_EQ(r.value().value().getAsNum(), 0);
}

TEST(ScriptLibrary, Atan2) {
    using ::testing::FloatEq;

    const std::string input = "return atan2(5, 5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(45));
}

TEST(ScriptLibrary, MinArray) {
    using ::testing::FloatEq;

    const std::string input = "return min([45, -30, 3.5]);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(-30.f));
}

TEST(ScriptLibrary, Min) {
    using ::testing::FloatEq;

    const std::string input = "return min(45, -30, 3.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(-30.f));
}

TEST(ScriptLibrary, MaxArray) {
    using ::testing::FloatEq;

    const std::string input = "return max([45, -30, 3.5]);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(45.f));
}

TEST(ScriptLibrary, Max) {
    using ::testing::FloatEq;

    const std::string input = "return max(45, -30, 3.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(45.f));
}

TEST(ScriptLibrary, SumArray) {
    using ::testing::FloatEq;

    const std::string input = "return sum([45, -30, 3.5]);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(18.5f));
}

TEST(ScriptLibrary, Sum) {
    using ::testing::FloatEq;

    const std::string input = "return sum(45, -30, 3.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(18.5f));
}

TEST(Value, FindSuccess) {
    using ::testing::FloatEq;

    Script script("ls = [1, 3, 5]; return ls.find(3);");
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(1.f));
}

TEST(Value, FindFail) {
    using ::testing::FloatEq;

    Script script("ls = [1, 3, 5]; return ls.find(30);");
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TNumeric);
    ASSERT_THAT(r.value().value().getAsNum(), FloatEq(-1.f));
}

} // namespace script
} // namespace bl
