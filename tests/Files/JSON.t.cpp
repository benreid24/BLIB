#include <BENG/Files/JSON.hpp>

#include <BENG/Files/FileUtil.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace bg
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

TEST(JSON, BasicGroup) {
    const std::string json = "{ \"num\": 123.45, \"str\": \"hello\", \"b\": true }";
    std::stringstream stream(json);

    Group root = JSON::loadFromStream(stream);
    EXPECT_TRUE(root.hasField("num"));
    EXPECT_TRUE(root.hasField("str"));
    EXPECT_TRUE(root.hasField("b"));
    ASSERT_TRUE(root.getString("str"));
    ASSERT_TRUE(root.getNumeric("num"));
    ASSERT_TRUE(root.getBool("b"));
    EXPECT_FLOAT_EQ(123.45, root.getNumeric("num").value());
    EXPECT_EQ("hello", root.getString("str").value());
    EXPECT_EQ(true, root.getBool("b").value());
}

TEST(JSON, NestedGroup) {
    const std::string json =
        "{\"grp\":{\"deep\":{\"wogh\":12},\"list\": [1,2,3]}, \"b\": false}";

    Group root = JSON::loadFromString(json);
    ASSERT_TRUE(root.getGroup("grp"));
    ASSERT_TRUE(root.getGroup("grp/deep"));
    ASSERT_TRUE(root.getNumeric("grp/deep/wogh"));
    ASSERT_TRUE(root.getList("grp/list"));
    ASSERT_TRUE(root.getBool("b"));
    EXPECT_EQ(root.getNumeric("grp/deep/wogh").value(), 12);
    EXPECT_EQ(false, root.getBool("b").value());
    List list = root.getList("grp/list").value();
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(list[0].getAsNumeric().value(), 1);
    EXPECT_EQ(list[1].getAsNumeric().value(), 2);
    EXPECT_EQ(list[2].getAsNumeric().value(), 3);
}

TEST(JSON, GroupList) {
    const std::string json = "{\"l\":[{\"name\": 15}]}";

    Group root = JSON::loadFromString(json);
    ASSERT_TRUE(root.hasField("l"));
    ASSERT_TRUE(root.getField("l").value().getAsList());
    List list = root.getField("l").value().getAsList().value();
    ASSERT_EQ(list.size(), 1);
    ASSERT_TRUE(list[0].getAsGroup());
    Group nested = list[0].getAsGroup().value();
    ASSERT_TRUE(nested.hasField("name"));
    ASSERT_TRUE(nested.getField("name").value().getAsNumeric());
    EXPECT_EQ(nested.getNumeric("name").value(), 15);
}

TEST(JSON, Files) {
    const std::string json =
        "{ \"num\": 123.45, \"str\": \"hello\", \"b\": true, \"ls\": [1,2,3] }";
    const std::string filename = FileUtil::genTempName("json", "json");

    Group goodRoot = JSON::loadFromString(json);
    FileUtil::createDirectory("json");
    JSON::saveToFile(filename, goodRoot);
    Group root = JSON::loadFromFile(filename);
    FileUtil::deleteFile(filename);

    EXPECT_TRUE(root.hasField("num"));
    EXPECT_TRUE(root.hasField("str"));
    EXPECT_TRUE(root.hasField("b"));
    ASSERT_TRUE(root.getString("str"));
    ASSERT_TRUE(root.getNumeric("num"));
    ASSERT_TRUE(root.getBool("b"));
    ASSERT_TRUE(root.getList("ls"));
    EXPECT_FLOAT_EQ(123.45, root.getNumeric("num").value());
    EXPECT_EQ("hello", root.getString("str").value());
    EXPECT_EQ(true, root.getBool("b").value());
    EXPECT_EQ(3, root.getList("ls").value().size());
}

} // namespace unittest
} // namespace json
} // namespace bg