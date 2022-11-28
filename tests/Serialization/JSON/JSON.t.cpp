#include <BLIB/Serialization/JSON/JSON.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace json
{
namespace unittest
{
TEST(JSON, BoolValue) {
    Value val(true);
    EXPECT_EQ(Value::Type::Bool, val.getType());
    ASSERT_NE(val.getAsBool(), nullptr);
    EXPECT_TRUE(*val.getAsBool());
    val = false;
    EXPECT_EQ(Value::Type::Bool, val.getType());
    ASSERT_NE(val.getAsBool(), nullptr);
    EXPECT_FALSE(*val.getAsBool());
}

TEST(JSON, IntegerValue) {
    Value val(15);
    EXPECT_EQ(Value::Type::Integer, val.getType());
    ASSERT_NE(val.getAsInteger(), nullptr);
    EXPECT_EQ(*val.getAsInteger(), 15);
    val = 32;
    EXPECT_EQ(Value::Type::Integer, val.getType());
    ASSERT_NE(val.getAsInteger(), nullptr);
    EXPECT_EQ(*val.getAsInteger(), 32);
    EXPECT_EQ(val.getNumericAsFloat(), 32.f);
    EXPECT_EQ(val.getNumericAsInteger(), 32);
}

TEST(JSON, FloatValue) {
    Value val(15.5f);
    EXPECT_EQ(Value::Type::Float, val.getType());
    ASSERT_NE(val.getAsFloat(), nullptr);
    EXPECT_EQ(*val.getAsFloat(), 15.5f);
    val = -32.4f;
    EXPECT_EQ(Value::Type::Float, val.getType());
    ASSERT_NE(val.getAsFloat(), nullptr);
    EXPECT_EQ(*val.getAsFloat(), -32.4f);
    EXPECT_EQ(val.getNumericAsFloat(), -32.4f);
    EXPECT_EQ(val.getNumericAsInteger(), -32);
}

TEST(JSON, StringValue) {
    Value val("hello");
    EXPECT_EQ(Value::Type::String, val.getType());
    ASSERT_NE(val.getAsString(), nullptr);
    EXPECT_EQ(*val.getAsString(), "hello");
    val = "world";
    EXPECT_EQ(Value::Type::String, val.getType());
    ASSERT_NE(val.getAsString(), nullptr);
    EXPECT_EQ(*val.getAsString(), "world");
}

TEST(JSON, ListValue) {
    List list;
    Value val(list);
    EXPECT_EQ(Value::Type::List, val.getType());
    EXPECT_TRUE(val.getAsList());
}

TEST(JSON, GroupValue) {
    Group group;
    Value val(group);
    EXPECT_EQ(Value::Type::Group, val.getType());
    EXPECT_TRUE(val.getAsGroup());
}

TEST(JSON, BasicGroup) {
    const std::string json = "{ \"num\": 123.45, \"str\": \"hello\", \"b\": true }";
    std::stringstream stream(json);

    Group root;
    ASSERT_TRUE(loadFromStream(stream, root));
    EXPECT_TRUE(root.hasField("num"));
    EXPECT_TRUE(root.hasField("str"));
    EXPECT_TRUE(root.hasField("b"));
    ASSERT_TRUE(root.getString("str"));
    ASSERT_TRUE(root.getBool("b"));
    EXPECT_FLOAT_EQ(123.45f, root.getFloat("num"));
    EXPECT_EQ("hello", *root.getString("str"));
    EXPECT_EQ(true, *root.getBool("b"));
}

TEST(JSON, NestedGroup) {
    std::stringstream json("{\"grp\":{\"deep\":{\"wogh\":12},\"list\": [1,2,3]}, \"b\": false}");

    Group root;
    ASSERT_TRUE(loadFromStream(json, root));
    ASSERT_TRUE(root.getGroup("grp"));
    ASSERT_TRUE(root.getGroup("grp/deep"));
    ASSERT_TRUE(root.getList("grp/list"));
    ASSERT_TRUE(root.getBool("b"));
    EXPECT_EQ(root.getInteger("grp/deep/wogh"), 12);
    EXPECT_EQ(false, *root.getBool("b"));
    const List& list = *root.getList("grp/list");
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(*list[0].getAsInteger(), 1);
    EXPECT_EQ(list[1].getNumericAsInteger(), 2);
    EXPECT_EQ(*list[2].getAsInteger(), 3);
}

TEST(JSON, GroupList) {
    std::stringstream json("{\"l\":[{\"name\": 15}]}");

    Group root;
    ASSERT_TRUE(loadFromStream(json, root));
    ASSERT_TRUE(root.hasField("l"));
    ASSERT_NE(root.getField("l")->getAsList(), nullptr);
    const List& list = *root.getField("l")->getAsList();
    ASSERT_EQ(list.size(), 1);
    ASSERT_TRUE(list[0].getAsGroup());
    const Group& nested = *list[0].getAsGroup();
    ASSERT_TRUE(nested.hasField("name"));
    EXPECT_EQ(nested.getInteger("name"), 15);
}

TEST(JSON, Files) {
    std::stringstream json("{ \"num\": 123.45, \"str\": \"hello\", \"b\": true, \"ls\": [1,2,3] }");
    const std::string filename = util::FileUtil::genTempName("json", "json");

    Group goodRoot;
    ASSERT_TRUE(loadFromStream(json, goodRoot));
    util::FileUtil::createDirectory("json");
    ASSERT_TRUE(saveToFile(filename, goodRoot));
    Group root;
    ASSERT_TRUE(loadFromFile(filename, root));
    util::FileUtil::deleteFile(filename);

    EXPECT_TRUE(root.hasField("num"));
    EXPECT_TRUE(root.hasField("str"));
    EXPECT_TRUE(root.hasField("b"));
    ASSERT_TRUE(root.getString("str"));
    ASSERT_TRUE(root.getBool("b"));
    ASSERT_TRUE(root.getList("ls"));
    EXPECT_FLOAT_EQ(123.45f, root.getFloat("num"));
    EXPECT_EQ("hello", *root.getString("str"));
    EXPECT_EQ(true, *root.getBool("b"));
    EXPECT_EQ(3, root.getList("ls")->size());
}

} // namespace unittest
} // namespace json
} // namespace serial
} // namespace bl
