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

    Serializer<bool>::serializeInto(result, "bool", false);
    Serializer<int>::serializeInto(result, "int", 8237);
    Serializer<float>::serializeInto(result, "float", -45.75);
    Serializer<std::string>::serializeInto(result, "string", "hello");
    Serializer<std::vector<int>>::serializeInto(result, "vec", {1, 2, 3});
    Serializer<decltype(map)>::serializeInto(result, "map", map);
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

TEST(JsonSerializer, ScriptValues) {
    using S = Serializer<script::Value>;

    script::Value b(true);
    b.setProperty("p1", script::Value(5.f));
    b.setProperty("p2", script::Value("hello"));
    b.setProperty("p3", script::Value(-100));
    script::Value n(56.f);
    script::Value s("Hello World");
    script::Value l(script::ArrayValue({b, n, s}));

    script::Value read;
    ASSERT_TRUE(S::deserialize(read, S::serialize(b)));
    ASSERT_EQ(read.value().getType(), script::PrimitiveValue::TBool);
    EXPECT_EQ(read.value().getAsBool(), true);
    script::ReferenceValue p = read.getProperty("p1", false);
    ASSERT_EQ(p.deref().value().getType(), script::PrimitiveValue::TFloat);
    EXPECT_EQ(p.deref().value().getAsFloat(), 5.f);
    p = read.getProperty("p2", false);
    ASSERT_EQ(p.deref().value().getType(), script::PrimitiveValue::TString);
    EXPECT_EQ(p.deref().value().getAsString(), "hello");
    p = read.getProperty("p3", false);
    ASSERT_EQ(p.deref().value().getType(), script::PrimitiveValue::TInteger);
    EXPECT_EQ(p.deref().value().getAsInt(), -100);

    ASSERT_TRUE(S::deserialize(read, S::serialize(n)));
    ASSERT_EQ(read.value().getType(), script::PrimitiveValue::TFloat);
    EXPECT_EQ(read.value().getAsFloat(), 56.f);

    ASSERT_TRUE(S::deserialize(read, S::serialize(s)));
    ASSERT_EQ(read.value().getType(), script::PrimitiveValue::TString);
    EXPECT_EQ(read.value().getAsString(), "Hello World");

    ASSERT_TRUE(S::deserialize(read, S::serialize(l)));
    ASSERT_EQ(read.value().getType(), script::PrimitiveValue::TArray);
    const auto& arr = read.value().getAsArray();
    ASSERT_EQ(arr.size(), 3);
    ASSERT_EQ(arr[0].value().getType(), script::PrimitiveValue::TBool);
    EXPECT_EQ(arr[0].value().getAsBool(), true);
    ASSERT_EQ(arr[1].value().getType(), script::PrimitiveValue::TFloat);
    EXPECT_EQ(arr[1].value().getAsFloat(), 56.f);
    ASSERT_EQ(arr[2].value().getType(), script::PrimitiveValue::TString);
    EXPECT_EQ(arr[2].value().getAsString(), "Hello World");
}

TEST(JsonSerializer, Pointers) {
    std::uint32_t testVal  = 35343;
    std::string testString = "hello fish";

    std::uint32_t readVal;
    EXPECT_TRUE(Serializer<std::uint32_t*>::deserialize(
        &readVal, Serializer<std::uint32_t*>::serialize(&testVal)));
    EXPECT_EQ(readVal, testVal);

    std::string readString;
    EXPECT_TRUE(Serializer<std::string*>::deserialize(
        &readString, Serializer<std::string*>::serialize(&testString)));
    EXPECT_EQ(readString, testString);
}

} // namespace unittest
} // namespace json
} // namespace serial
} // namespace bl