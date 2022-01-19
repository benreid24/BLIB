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

TEST(ScriptLibrary, RandomInt) {
    const std::string input = "return randomInt(0, 10);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TInteger);
    ASSERT_LE(v.value().getAsInt(), 10);
    ASSERT_GE(v.value().getAsInt(), 0);
}

TEST(ScriptLibrary, RandomFloat) {
    const std::string input = "return randomFloat(0.0, 10.0);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TFloat);
    ASSERT_LE(v.value().getAsFloat(), 10.f);
    ASSERT_GE(v.value().getAsFloat(), 0.f);
}

TEST(ScriptLibrary, Run) {
    const std::string input = "return run(\"return 5;\", false);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    const Value v = r.value();
    ASSERT_EQ(v.value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(v.value().getAsInt(), 5);
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

TEST(ScriptLibrary, IntStr) {
    const std::string input = "return int(\"5\");";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 5);
}

TEST(ScriptLibrary, IntFloat) {
    const std::string input = "return int(5.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 5);
}

TEST(ScriptLibrary, FloatStr) {
    const std::string input = "return float(\"5.5\");";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 5.5f);
}

TEST(ScriptLibrary, FloatInt) {
    const std::string input = "return float(5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 5.f);
}

TEST(ScriptLibrary, Sqrt) {
    const std::string input = "return sqrt(16);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 4.f);
}

TEST(ScriptLibrary, AbsInt) {
    const std::string input = "return abs(-16);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 16);
}

TEST(ScriptLibrary, AbsFloat) {
    const std::string input = "return abs(-16.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 16.5f);
}

TEST(ScriptLibrary, Round) {
    const std::string input = "return round(16.1) + round(16.7);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 33);
}

TEST(ScriptLibrary, Floor) {
    const std::string input = "return floor(16.9);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 16);
}

TEST(ScriptLibrary, Ceil) {
    const std::string input = "return ceil(16.1);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 17);
}

TEST(ScriptLibrary, Sin) {
    const std::string input = "return sin(90);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 1.f);
}

TEST(ScriptLibrary, Cos) {
    const std::string input = "return cos(0);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 1.f);
}

TEST(ScriptLibrary, Tan) {
    const std::string input = "return tan(0);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_EQ(r.value().value().getAsFloat(), 0.f);
}

TEST(ScriptLibrary, Atan2) {
    using ::testing::FloatEq;

    const std::string input = "return atan2(5, 5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_THAT(r.value().value().getAsFloat(), FloatEq(45.f));
}

TEST(ScriptLibrary, MinArray) {
    using ::testing::FloatEq;

    const std::string input = "return min([45, -30, 3.5]);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), -30);
}

TEST(ScriptLibrary, Min) {
    using ::testing::FloatEq;

    const std::string input = "return min(45, -30, 3.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), -30);
}

TEST(ScriptLibrary, MaxArray) {
    using ::testing::FloatEq;

    const std::string input = "return max([45, -30, 3.5]);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 45);
}

TEST(ScriptLibrary, Max) {
    using ::testing::FloatEq;

    const std::string input = "return max(45.5, -30, 3.5);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_THAT(r.value().value().getAsFloat(), FloatEq(45.5f));
}

TEST(ScriptLibrary, SumArray) {
    using ::testing::FloatEq;

    const std::string input = "return sum([45, -30, 3.5]);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TFloat);
    ASSERT_THAT(r.value().value().getAsFloat(), FloatEq(18.5f));
}

TEST(ScriptLibrary, Sum) {
    using ::testing::FloatEq;

    const std::string input = "return sum(45, -30, 3);";
    Script script(input);
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 18);
}

TEST(Value, FindSuccess) {
    using ::testing::FloatEq;

    Script script("ls = [1, 3, 5]; return ls.find(3);");
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), 1);
}

TEST(Value, FindFail) {
    using ::testing::FloatEq;

    Script script("ls = [1, 3, 5]; return ls.find(30);");
    ASSERT_TRUE(script.valid());

    std::optional<Value> r = script.run();
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().value().getType(), PrimitiveValue::TInteger);
    ASSERT_EQ(r.value().value().getAsInt(), -1);
}

} // namespace script
} // namespace bl
