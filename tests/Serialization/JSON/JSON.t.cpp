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
    EXPECT_EQ(Value::TBool, val.getType());
    ASSERT_NE(val.getAsBool(), nullptr);
    EXPECT_TRUE(*val.getAsBool());
    val = false;
    EXPECT_EQ(Value::TBool, val.getType());
    ASSERT_NE(val.getAsBool(), nullptr);
    EXPECT_FALSE(*val.getAsBool());
}

TEST(JSON, NumericalValue) {
    Value val(15.5f);
    EXPECT_EQ(Value::TNumeric, val.getType());
    ASSERT_NE(val.getAsNumeric(), nullptr);
    EXPECT_EQ(*val.getAsNumeric(), 15.5f);
    val = 32.3f;
    EXPECT_EQ(Value::TNumeric, val.getType());
    ASSERT_NE(val.getAsNumeric(), nullptr);
    EXPECT_EQ(*val.getAsNumeric(), 32.3f);
}

TEST(JSON, StringValue) {
    Value val("hello");
    EXPECT_EQ(Value::TString, val.getType());
    ASSERT_NE(val.getAsString(), nullptr);
    EXPECT_EQ(*val.getAsString(), "hello");
    val = "world";
    EXPECT_EQ(Value::TString, val.getType());
    ASSERT_NE(val.getAsString(), nullptr);
    EXPECT_EQ(*val.getAsString(), "world");
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

    Group root = loadFromStream(stream);
    EXPECT_TRUE(root.hasField("num"));
    EXPECT_TRUE(root.hasField("str"));
    EXPECT_TRUE(root.hasField("b"));
    ASSERT_TRUE(root.getString("str"));
    ASSERT_TRUE(root.getNumeric("num"));
    ASSERT_TRUE(root.getBool("b"));
    EXPECT_FLOAT_EQ(123.45, *root.getNumeric("num"));
    EXPECT_EQ("hello", *root.getString("str"));
    EXPECT_EQ(true, *root.getBool("b"));
}

TEST(JSON, NestedGroup) {
    const std::string json = "{\"grp\":{\"deep\":{\"wogh\":12},\"list\": [1,2,3]}, \"b\": false}";

    Group root = loadFromString(json);
    ASSERT_TRUE(root.getGroup("grp"));
    ASSERT_TRUE(root.getGroup("grp/deep"));
    ASSERT_TRUE(root.getNumeric("grp/deep/wogh"));
    ASSERT_TRUE(root.getList("grp/list"));
    ASSERT_TRUE(root.getBool("b"));
    EXPECT_EQ(*root.getNumeric("grp/deep/wogh"), 12);
    EXPECT_EQ(false, *root.getBool("b"));
    const List& list = *root.getList("grp/list");
    ASSERT_EQ(list.size(), 3);
    EXPECT_EQ(*list[0].getAsNumeric(), 1);
    EXPECT_EQ(*list[1].getAsNumeric(), 2);
    EXPECT_EQ(*list[2].getAsNumeric(), 3);
}

TEST(JSON, GroupList) {
    const std::string json = "{\"l\":[{\"name\": 15}]}";

    Group root = loadFromString(json);
    ASSERT_TRUE(root.hasField("l"));
    ASSERT_NE(root.getField("l")->getAsList(), nullptr);
    const List& list = *root.getField("l")->getAsList();
    ASSERT_EQ(list.size(), 1);
    ASSERT_TRUE(list[0].getAsGroup());
    const Group& nested = *list[0].getAsGroup();
    ASSERT_TRUE(nested.hasField("name"));
    ASSERT_NE(nested.getField("name")->getAsNumeric(), nullptr);
    EXPECT_EQ(*nested.getNumeric("name"), 15);
}

TEST(JSON, Files) {
    const std::string json =
        "{ \"num\": 123.45, \"str\": \"hello\", \"b\": true, \"ls\": [1,2,3] }";
    const std::string filename = util::FileUtil::genTempName("json", "json");

    Group goodRoot = loadFromString(json);
    util::FileUtil::createDirectory("json");
    saveToFile(filename, goodRoot);
    Group root = loadFromFile(filename);
    util::FileUtil::deleteFile(filename);

    EXPECT_TRUE(root.hasField("num"));
    EXPECT_TRUE(root.hasField("str"));
    EXPECT_TRUE(root.hasField("b"));
    ASSERT_TRUE(root.getString("str"));
    ASSERT_TRUE(root.getNumeric("num"));
    ASSERT_TRUE(root.getBool("b"));
    ASSERT_TRUE(root.getList("ls"));
    EXPECT_FLOAT_EQ(123.45, *root.getNumeric("num"));
    EXPECT_EQ("hello", *root.getString("str"));
    EXPECT_EQ(true, *root.getBool("b"));
    EXPECT_EQ(3, root.getList("ls")->size());
}

} // namespace unittest
} // namespace json
} // namespace serial
} // namespace bl
