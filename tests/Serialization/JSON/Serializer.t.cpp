#include <BLIB/Serialization/JSON.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace json
{
namespace unittest
{
TEST(JsonSerializer, BasicTypes) {
    bool b = true;
    EXPECT_TRUE(Serializer<bool>::deserialize(b, Serializer<bool>::serialize(true)));
    EXPECT_EQ(b, true);

    int i = 6784578;
    EXPECT_TRUE(Serializer<int>::deserialize(i, Serializer<int>::serialize(123456)));
    EXPECT_EQ(i, 123456);

    float f = 24243.24;
    EXPECT_TRUE(Serializer<float>::deserialize(f, Serializer<float>::serialize(-123.5f)));
    EXPECT_LE(std::abs(f + 123.5), 0.1f);

    std::string s = "hello world";
    EXPECT_TRUE(
        Serializer<std::string>::deserialize(s, Serializer<std::string>::serialize("yo yo yo")));
    EXPECT_EQ(s, "yo yo yo");

    std::vector<std::string> v;
    EXPECT_TRUE(Serializer<std::vector<std::string>>::deserialize(
        v, Serializer<std::vector<std::string>>::serialize({"1", "2", "3"})));
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], "1");
    EXPECT_EQ(v[1], "2");
    EXPECT_EQ(v[2], "3");

    std::unordered_map<std::string, int> m;
    std::unordered_map<std::string, int> expected;
    expected["k1"] = 5;
    expected["k2"] = 15;
    expected["k3"] = -5;
    EXPECT_TRUE(
        Serializer<decltype(m)>::deserialize(m, Serializer<decltype(m)>::serialize(expected)));
    ASSERT_EQ(m.size(), expected.size());
    for (const auto& p : expected) {
        const auto it = m.find(p.first);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(p.second, it->second);
    }
}

TEST(JsonSerializer, SerializeFromTo) {
    Group result;

    std::unordered_map<std::string, int> map;
    map["k1"] = 5;
    map["k1"] = 6;
    map["k1"] = 8;

    EXPECT_TRUE(Serializer<bool>::serializeInto(result, "bool", false));
    EXPECT_TRUE(Serializer<int>::serializeInto(result, "int", 8237));
    EXPECT_TRUE(Serializer<float>::serializeInto(result, "float", -45.75));
    EXPECT_TRUE(Serializer<std::string>::serializeInto(result, "string", "hello"));
    EXPECT_TRUE(Serializer<std::vector<int>>::serializeInto(result, "vec", {1, 2, 3}));
    EXPECT_TRUE(Serializer<decltype(map)>::serializeInto(result, "map", map));
    const Value val(result);

    bool b = true;
    EXPECT_TRUE(Serializer<bool>::deserializeFrom(val, "bool", b));
    EXPECT_EQ(b, false);

    int i = 724273;
    EXPECT_TRUE(Serializer<int>::deserializeFrom(val, "int", i));
    EXPECT_EQ(i, 8237);

    float f = 34324.3434;
    EXPECT_TRUE(Serializer<float>::deserializeFrom(val, "float", f));
    EXPECT_LE(std::abs(f + 45.75), 0.1f);

    std::string s;
    EXPECT_TRUE(Serializer<std::string>::deserializeFrom(val, "string", s));
    EXPECT_EQ(s, "hello");

    std::vector<int> v;
    EXPECT_TRUE(Serializer<std::vector<int>>::deserializeFrom(val, "vec", v));
    ASSERT_EQ(v.size(), 3);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[1], 2);
    EXPECT_EQ(v[2], 3);

    std::unordered_map<std::string, int> m;
    EXPECT_TRUE(Serializer<decltype(m)>::deserializeFrom(val, "map", m));
    EXPECT_EQ(m.size(), map.size());
    for (const auto& p : map) {
        const auto it = m.find(p.first);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(it->second, p.second);
    }
}

} // namespace unittest
} // namespace json
} // namespace serial
} // namespace bl
