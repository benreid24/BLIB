#include <BLIB/Files/JSON.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace file
{
namespace json
{
namespace unittest
{
TEST(JsonSerializableField, Bool) {
    SerializableObject owner;
    SerializableField<bool> field("field", owner);

    field.setValue(true);
    const Value v1 = field.serialize();
    ASSERT_TRUE(field.deserialize(v1));
    EXPECT_EQ(field.getValue(), true);

    field.setValue(false);
    const Value v2 = field.serialize();
    ASSERT_TRUE(field.deserialize(v2));
    EXPECT_EQ(field.getValue(), false);
}

TEST(JsonSerializableField, Int) {
    SerializableObject owner;
    SerializableField<int> field("field", owner);

    field.setValue(42);
    const Value v1 = field.serialize();
    ASSERT_TRUE(field.deserialize(v1));
    EXPECT_EQ(field.getValue(), 42);
}

TEST(JsonSerializableField, Float) {
    SerializableObject owner;
    SerializableField<float> field("field", owner);

    field.setValue(69.42);
    const Value v1 = field.serialize();
    ASSERT_TRUE(field.deserialize(v1));
    EXPECT_NEAR(field.getValue(), 69.42, 0.01);
}

TEST(JsonSerializableField, String) {
    SerializableObject owner;
    SerializableField<std::string> field("field", owner);

    field.setValue("hello");
    const Value v1 = field.serialize();
    ASSERT_TRUE(field.deserialize(v1));
    EXPECT_EQ(field.getValue(), "hello");
}

TEST(JsonSerializableField, Vector) {
    SerializableObject owner;
    SerializableField<std::vector<int>> field("field", owner);

    field.setValue({5, 10});
    const Value v1 = field.serialize();
    ASSERT_TRUE(field.deserialize(v1));
    EXPECT_EQ(field.getValue()[0], 5);
    EXPECT_EQ(field.getValue()[1], 10);
}

TEST(JsonSerializableField, Map) {
    SerializableObject owner;
    SerializableField<std::unordered_map<std::string, int>> field("field", owner);

    field.setValue({{"k1", 5}, {"k2", 10}});
    const Value v1 = field.serialize();
    ASSERT_TRUE(field.deserialize(v1));
    EXPECT_EQ(field.getValue().at("k1"), 5);
    EXPECT_EQ(field.getValue().at("k2"), 10);
}

} // namespace unittest
} // namespace json
} // namespace file
} // namespace bl
