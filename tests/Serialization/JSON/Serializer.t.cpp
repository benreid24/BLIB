#include <BLIB/Serialization.hpp>
#include <BLIB/Serialization/JSON/Serializer.hpp>
#include <glm/detail/type_quat.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace json
{
namespace unittest
{
struct TestBoi {
    bool bfield;
    int ifield;
    float ffield;
    std::string sfield;
    std::vector<int> vfield;
    std::unordered_map<std::string, std::string> smfield;
    std::unordered_map<int, std::string> imfield;

    TestBoi() = default;

    TestBoi(int)
    : bfield(true)
    , ifield(56)
    , ffield(-123.5f)
    , sfield("test string") {
        vfield.reserve(4);
        vfield.push_back(1);
        vfield.push_back(-6);
        vfield.push_back(17);
        vfield.push_back(13343);

        smfield["df1"] = "test value 1";
        smfield["df2"] = "test value 2";

        imfield[5]  = "field 5";
        imfield[-5] = "field -5";
    }
};
} // namespace unittest
} // namespace json
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<serial::json::unittest::TestBoi> {
    inline static const auto spec = makeSpec<serial::json::unittest::TestBoi>(
        "TestBoi",
        memberList(defineMember(1, "bfield", &serial::json::unittest::TestBoi::bfield),
                   defineMember(2, "ifield", &serial::json::unittest::TestBoi::ifield),
                   defineMember(3, "ffield", &serial::json::unittest::TestBoi::ffield),
                   defineMember(4, "sfield", &serial::json::unittest::TestBoi::sfield),
                   defineMember(5, "vfield", &serial::json::unittest::TestBoi::vfield),
                   defineMember(6, "smfield", &serial::json::unittest::TestBoi::smfield),
                   defineMember(7, "imfield", &serial::json::unittest::TestBoi::imfield)));
};
} // namespace refl

namespace serial
{
namespace json
{
namespace unittest
{

using TestySerial = Serializer<TestBoi>;

TEST(JsonSerializer, BasicTypes) {
    bool b = true;
    EXPECT_TRUE(Serializer<bool>::deserialize(b, Serializer<bool>::serialize(true)));
    EXPECT_EQ(b, true);

    int i = 6784578;
    EXPECT_TRUE(Serializer<int>::deserialize(i, Serializer<int>::serialize(123456)));
    EXPECT_EQ(i, 123456);

    float f = 24243.24f;
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

    float f = 34324.3434f;
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

TEST(JsonSerializer, DirectSerialization) {
    stream::OutputStream out(1024);

    TestBoi good(5);
    ASSERT_TRUE(TestySerial::serializeStream(out, good, 4, 0));

    stream::InputStream in(out.getBuffer());
    TestBoi read;
    ASSERT_TRUE(TestySerial::deserializeStream(in, read));

    EXPECT_EQ(good.bfield, read.bfield);
    EXPECT_EQ(good.ifield, read.ifield);
    EXPECT_EQ(good.ffield, read.ffield);
    EXPECT_EQ(good.sfield, read.sfield);

    EXPECT_EQ(good.smfield.size(), read.smfield.size());
    for (const auto& pair : good.smfield) {
        const auto it = read.smfield.find(pair.first);
        ASSERT_NE(it, read.smfield.end()) << pair.first;
        EXPECT_EQ(pair.second, it->second);
    }

    EXPECT_EQ(good.imfield.size(), read.imfield.size());
    for (const auto& pair : good.imfield) {
        const auto it = read.imfield.find(pair.first);
        ASSERT_NE(it, read.imfield.end()) << pair.first;
        EXPECT_EQ(pair.second, it->second);
    }
}

TEST(JsonSerializer, Double) {
    double value  = 2.71828;
    double result = 0.0;
    EXPECT_TRUE(Serializer<double>::deserialize(result, Serializer<double>::serialize(value)));
    EXPECT_NEAR(result, value, 0.0001);
}

TEST(JsonSerializer, Enum) {
    enum class Color : int { Red = 1, Green = 2, Blue = 3 };

    Color result = Color::Red;
    EXPECT_TRUE(Serializer<Color>::deserialize(result, Serializer<Color>::serialize(Color::Blue)));
    EXPECT_EQ(result, Color::Blue);
}

TEST(JsonSerializer, CArray) {
    const int arr[3] = {10, 20, 30};
    int read[3]      = {};
    EXPECT_TRUE(Serializer<int[3]>::deserialize(read, Serializer<int[3]>::serialize(arr)));
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(JsonSerializer, StdArray) {
    const std::array<int, 3> arr = {10, 20, 30};
    std::array<int, 3> read      = {};
    EXPECT_TRUE((Serializer<std::array<int, 3>>::deserialize(
        read, Serializer<std::array<int, 3>>::serialize(arr))));
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(JsonSerializer, UnorderedSet) {
    const std::unordered_set<std::string> set = {"apple", "banana", "cherry"};
    std::unordered_set<std::string> read;
    EXPECT_TRUE(Serializer<std::unordered_set<std::string>>::deserialize(
        read, Serializer<std::unordered_set<std::string>>::serialize(set)));
    ASSERT_EQ(read.size(), set.size());
    EXPECT_NE(read.find("apple"), read.end());
    EXPECT_NE(read.find("banana"), read.end());
    EXPECT_NE(read.find("cherry"), read.end());
}

TEST(JsonSerializer, SFMLVector2) {
    const sf::Vector2f v(3.5f, -7.0f);
    sf::Vector2f result;
    EXPECT_TRUE(
        Serializer<sf::Vector2f>::deserialize(result, Serializer<sf::Vector2f>::serialize(v)));
    EXPECT_NEAR(result.x, v.x, 0.001f);
    EXPECT_NEAR(result.y, v.y, 0.001f);
}

TEST(JsonSerializer, SFMLVector3) {
    const sf::Vector3f v(1.0f, 2.0f, 3.0f);
    sf::Vector3f result;
    EXPECT_TRUE(
        Serializer<sf::Vector3f>::deserialize(result, Serializer<sf::Vector3f>::serialize(v)));
    EXPECT_NEAR(result.x, v.x, 0.001f);
    EXPECT_NEAR(result.y, v.y, 0.001f);
    EXPECT_NEAR(result.z, v.z, 0.001f);
}

TEST(JsonSerializer, SFMLRect) {
    const sf::FloatRect r({10.f, 20.f}, {100.f, 200.f});
    sf::FloatRect result;
    EXPECT_TRUE(
        Serializer<sf::FloatRect>::deserialize(result, Serializer<sf::FloatRect>::serialize(r)));
    EXPECT_NEAR(result.position.x, r.position.x, 0.001f);
    EXPECT_NEAR(result.position.y, r.position.y, 0.001f);
    EXPECT_NEAR(result.size.x, r.size.x, 0.001f);
    EXPECT_NEAR(result.size.y, r.size.y, 0.001f);
}

TEST(JsonSerializer, GlmVec2) {
    const glm::vec2 v(1.5f, 2.5f);
    glm::vec2 result(0.f);
    EXPECT_TRUE((Serializer<glm::vec2>::deserialize(result, Serializer<glm::vec2>::serialize(v))));
    EXPECT_NEAR(result.x, v.x, 0.001f);
    EXPECT_NEAR(result.y, v.y, 0.001f);
}

TEST(JsonSerializer, GlmVec3) {
    const glm::vec3 v(1.0f, 2.0f, 3.0f);
    glm::vec3 result(0.f);
    EXPECT_TRUE((Serializer<glm::vec3>::deserialize(result, Serializer<glm::vec3>::serialize(v))));
    EXPECT_NEAR(result.x, v.x, 0.001f);
    EXPECT_NEAR(result.y, v.y, 0.001f);
    EXPECT_NEAR(result.z, v.z, 0.001f);
}

TEST(JsonSerializer, GlmVec4) {
    const glm::vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    glm::vec4 result(0.f);
    EXPECT_TRUE((Serializer<glm::vec4>::deserialize(result, Serializer<glm::vec4>::serialize(v))));
    EXPECT_NEAR(result.x, v.x, 0.001f);
    EXPECT_NEAR(result.y, v.y, 0.001f);
    EXPECT_NEAR(result.z, v.z, 0.001f);
    EXPECT_NEAR(result.w, v.w, 0.001f);
}

TEST(JsonSerializer, GlmQuat) {
    const glm::quat q(1.0f, 0.5f, 0.25f, 0.1f);
    glm::quat result(0.f, 0.f, 0.f, 0.f);
    EXPECT_TRUE((Serializer<glm::quat>::deserialize(result, Serializer<glm::quat>::serialize(q))));
    EXPECT_NEAR(result.x, q.x, 0.001f);
    EXPECT_NEAR(result.y, q.y, 0.001f);
    EXPECT_NEAR(result.z, q.z, 0.001f);
    EXPECT_NEAR(result.w, q.w, 0.001f);
}

TEST(JsonSerializer, GlmMat4) {
    glm::mat4 m(0.f);
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) { m[c][r] = static_cast<float>(c * 4 + r); }
    }
    glm::mat4 result(0.f);
    EXPECT_TRUE((Serializer<glm::mat4>::deserialize(result, Serializer<glm::mat4>::serialize(m))));
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) { EXPECT_NEAR(result[c][r], m[c][r], 0.001f); }
    }
}

TEST(JsonSerializer, Vector2D) {
    ctr::Vector2D<int> grid;
    grid.setSize(2, 2);
    grid(0, 0) = 1;
    grid(0, 1) = 2;
    grid(1, 0) = 3;
    grid(1, 1) = 4;

    ctr::Vector2D<int> result;
    EXPECT_TRUE((Serializer<ctr::Vector2D<int>>::deserialize(
        result, Serializer<ctr::Vector2D<int>>::serialize(grid))));
    ASSERT_EQ(result.getWidth(), 2u);
    ASSERT_EQ(result.getHeight(), 2u);
    EXPECT_EQ(result(0, 0), grid(0, 0));
    EXPECT_EQ(result(0, 1), grid(0, 1));
    EXPECT_EQ(result(1, 0), grid(1, 0));
    EXPECT_EQ(result(1, 1), grid(1, 1));
}

TEST(JsonSerializer, Variant) {
    const std::variant<std::string, int> v(std::string("hello"));
    std::variant<std::string, int> result;
    EXPECT_TRUE((Serializer<std::variant<std::string, int>>::deserialize(
        result, Serializer<std::variant<std::string, int>>::serialize(v))));
    ASSERT_EQ(result.index(), v.index());
    EXPECT_EQ(std::get<std::string>(result), "hello");
}

TEST(JsonSerializer, Pair) {
    const std::pair<std::string, int> p("key", 42);
    std::pair<std::string, int> result;
    EXPECT_TRUE((Serializer<std::pair<std::string, int>>::deserialize(
        result, Serializer<std::pair<std::string, int>>::serialize(p))));
    EXPECT_EQ(result.first, p.first);
    EXPECT_EQ(result.second, p.second);
}

TEST(JsonSerializer, SFMLImage) {
    sf::Image image(sf::Vector2u{2, 2});
    image.setPixel({0, 0}, sf::Color(255, 0, 0, 255));
    image.setPixel({1, 0}, sf::Color(0, 255, 0, 255));
    image.setPixel({0, 1}, sf::Color(0, 0, 255, 255));
    image.setPixel({1, 1}, sf::Color(255, 255, 0, 255));

    sf::Image result;
    EXPECT_TRUE(
        Serializer<sf::Image>::deserialize(result, Serializer<sf::Image>::serialize(image)));
    ASSERT_EQ(result.getSize(), sf::Vector2u(2, 2));
    EXPECT_EQ(result.getPixel({0, 0}), sf::Color(255, 0, 0, 255));
    EXPECT_EQ(result.getPixel({1, 0}), sf::Color(0, 255, 0, 255));
    EXPECT_EQ(result.getPixel({0, 1}), sf::Color(0, 0, 255, 255));
    EXPECT_EQ(result.getPixel({1, 1}), sf::Color(255, 255, 0, 255));
}

TEST(JsonSerializer, Optional) {
    const std::optional<int> withValue(99);
    const std::optional<int> noValue;

    std::optional<int> rWith;
    EXPECT_TRUE((Serializer<std::optional<int>>::deserialize(
        rWith, Serializer<std::optional<int>>::serialize(withValue))));
    ASSERT_TRUE(rWith.has_value());
    EXPECT_EQ(*rWith, 99);

    std::optional<int> rNone;
    EXPECT_TRUE((Serializer<std::optional<int>>::deserialize(
        rNone, Serializer<std::optional<int>>::serialize(noValue))));
    EXPECT_FALSE(rNone.has_value());
}

} // namespace unittest
} // namespace json
} // namespace serial
} // namespace bl
