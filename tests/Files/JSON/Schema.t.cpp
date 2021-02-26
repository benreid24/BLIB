#include <BLIB/Files/JSON/Schema.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace json
{
namespace schema
{
namespace unittest
{
TEST(Schema, BasicTypesPass) {
    const std::string json = "{"
                             "\"str\": \"word\","
                             "\"num\": 15,"
                             "\"b\": true }";
    json::Group root = JSON::loadFromString(json);

    Schema schema;
    schema.addRequiredField("str", String::Any);
    schema.addRequiredField("num", Numeric::Any);
    schema.addRequiredField("b", Bool::Any);

    EXPECT_TRUE(schema.validate(root, true));
}

TEST(Schema, NumericBounds) {
    json::Value val = 0.5f;
    EXPECT_TRUE(Value(Numeric::Any).validate(val, true));
    EXPECT_TRUE(Value(Numeric::Positive).validate(val, true));
    EXPECT_FALSE(Value(Numeric::Negative).validate(val, true));

    val = -0.5f;
    EXPECT_TRUE(Value(Numeric::Any).validate(val, true));
    EXPECT_FALSE(Value(Numeric::Positive).validate(val, true));
    EXPECT_TRUE(Value(Numeric::Negative).validate(val, true));

    val = 15.0f;
    EXPECT_TRUE(Value(Numeric::Any).validate(val, true));
    EXPECT_FALSE(Value(Numeric({16, {}})).validate(val, true));
    EXPECT_TRUE(Value(Numeric({14, 16})).validate(val, true));
}

TEST(Schema, StringSet) {
    json::Value val = "hello";
    EXPECT_TRUE(Value(String::Any).validate(val, true));
    EXPECT_TRUE(Value(String({"hello"})).validate(val, true));
    EXPECT_TRUE(Value(String({"hello", "world"})).validate(val, true));
    EXPECT_FALSE(Value(String({"world"})).validate(val, true));
}

TEST(Schema, ListBounds) {
    SourceInfo s;
    json::List list = {json::Value(true), json::Value(false), json::Value(true)};
    EXPECT_TRUE(List(Bool::Any).validate(s, list, true));
    EXPECT_TRUE(List(Bool::Any, 2).validate(s, list, true));
    EXPECT_TRUE(List(Bool::Any, 1, 5).validate(s, list, true));
    EXPECT_FALSE(List(Bool::Any, 5).validate(s, list, true));
    EXPECT_FALSE(List(Bool::Any, 5, 10).validate(s, list, true));
    EXPECT_FALSE(List(Bool::Any, 1, 2).validate(s, list, true));
    EXPECT_FALSE(List(Bool::Any, 0, 1).validate(s, list, true));
}

TEST(Schema, ChoiceGroup) {
    Schema schema;
    schema.addChoiceField("one", Numeric::Any);
    schema.addChoiceField("two", Numeric::Any);
    schema.addChoiceField("three", Numeric::Any);

    json::Group root;
    root.addField("one", 15);
    EXPECT_TRUE(schema.validate(root, true));
    root.addField("extra", json::Value(true));
    EXPECT_TRUE(schema.validate(root, false));
    EXPECT_FALSE(schema.validate(root, true));

    json::Group root2;
    root2.addField("two", 32);
    EXPECT_TRUE(schema.validate(root2, true));
    root2.addField("three", 45);
    EXPECT_FALSE(schema.validate(root2, true));
}

TEST(Schema, RequiredFields) {
    Schema schema;
    schema.addRequiredField("one", String::Any);
    schema.addRequiredField("two", String::Any);
    schema.addRequiredField("three", String::Any);

    json::Group root;
    EXPECT_FALSE(schema.validate(root, true));
    root.addField("one", "woah");
    EXPECT_FALSE(schema.validate(root, true));
    root.addField("two", "glarb");
    EXPECT_FALSE(schema.validate(root, true));
    root.addField("three", "pass");
    EXPECT_TRUE(schema.validate(root, true));
}

TEST(Schema, OptionalFields) {
    Schema schema;
    schema.addRequiredField("one", String::Any);
    schema.addRequiredField("two", String::Any);
    schema.addRequiredField("three", String::Any);
    schema.addOptionalField("opt1", Bool::Any);
    schema.addOptionalField("opt2", Numeric::Any);

    json::Group root;
    root.addField("one", "woah");
    root.addField("two", "glarb");
    root.addField("three", "pass");
    EXPECT_TRUE(schema.validate(root, true));
    root.addField("opt1", json::Value(true));
    EXPECT_TRUE(schema.validate(root, true));
    root.addField("opt2", 92);
    EXPECT_TRUE(schema.validate(root, true));
    root.addField("extra", "oh no");
    EXPECT_TRUE(schema.validate(root, false));
    EXPECT_FALSE(schema.validate(root, true));
}

TEST(Schema, NestedGroup) {
    Schema nested;
    nested.addRequiredField("deep", Numeric::Any);
    Schema schema;
    schema.addRequiredField("nested", nested);

    json::Group root;
    json::Group deep;
    deep.addField("deep", 123);
    root.addField("nested", deep);
    EXPECT_TRUE(schema.validate(root, true));
}

TEST(Schema, TypeMismatches) {
    EXPECT_TRUE(Value(Bool::Any).validate(json::Value(true), true));
    EXPECT_FALSE(Value(Bool::Any).validate(123, true));
    EXPECT_FALSE(Value(Bool::Any).validate("hello", true));

    EXPECT_FALSE(Value(Numeric::Any).validate(json::Value(true), true));
    EXPECT_TRUE(Value(Numeric::Any).validate(123, true));
    EXPECT_FALSE(Value(Numeric::Any).validate("hello", true));

    EXPECT_FALSE(Value(String::Any).validate(json::Value(true), true));
    EXPECT_FALSE(Value(String::Any).validate(123, true));
    EXPECT_TRUE(Value(String::Any).validate("hello", true));
}

} // namespace unittest
} // namespace schema
} // namespace json
} // namespace bl