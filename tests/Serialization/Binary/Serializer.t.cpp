#include <BLIB/Logging.hpp>
#include <BLIB/Serialization/Binary.hpp>
#include <glm/detail/type_quat.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace binary
{
namespace unittest
{
TEST(BinarySerializer, Integers) {
    stream::OutputStream stream(1024);

    const std::uint8_t u8   = 123;
    const std::uint16_t u16 = 8734;
    const std::uint32_t u32 = 342432;
    const std::uint64_t u64 = 874657842353ULL;
    const std::int8_t i8    = -23;
    const std::int16_t i16  = -2342;
    const std::int32_t i32  = -2423232;
    const std::int64_t i64  = -2342323221LL;

    ASSERT_TRUE(Serializer<std::uint8_t>::serialize(stream, u8));
    ASSERT_TRUE(Serializer<std::uint16_t>::serialize(stream, u16));
    ASSERT_TRUE(Serializer<std::uint32_t>::serialize(stream, u32));
    ASSERT_TRUE(Serializer<std::uint64_t>::serialize(stream, u64));
    ASSERT_TRUE(Serializer<std::int8_t>::serialize(stream, i8));
    ASSERT_TRUE(Serializer<std::int16_t>::serialize(stream, i16));
    ASSERT_TRUE(Serializer<std::int32_t>::serialize(stream, i32));
    ASSERT_TRUE(Serializer<std::int64_t>::serialize(stream, i64));

    stream::InputStream in(stream.getBuffer());

    std::uint8_t u8a   = 123;
    std::uint16_t u16a = 8734;
    std::uint32_t u32a = 342432;
    std::uint64_t u64a = 874657842353ULL;
    std::int8_t i8a    = -23;
    std::int16_t i16a  = -2342;
    std::int32_t i32a  = -2423232;
    std::int64_t i64a  = -2342323221LL;

    ASSERT_TRUE(Serializer<std::uint8_t>::deserialize(in, u8a));
    ASSERT_TRUE(Serializer<std::uint16_t>::deserialize(in, u16a));
    ASSERT_TRUE(Serializer<std::uint32_t>::deserialize(in, u32a));
    ASSERT_TRUE(Serializer<std::uint64_t>::deserialize(in, u64a));
    ASSERT_TRUE(Serializer<std::int8_t>::deserialize(in, i8a));
    ASSERT_TRUE(Serializer<std::int16_t>::deserialize(in, i16a));
    ASSERT_TRUE(Serializer<std::int32_t>::deserialize(in, i32a));
    ASSERT_TRUE(Serializer<std::int64_t>::deserialize(in, i64a));

    EXPECT_EQ(u8, u8a);
    EXPECT_EQ(u16, u16a);
    EXPECT_EQ(u32, u32a);
    EXPECT_EQ(u64, u64a);
    EXPECT_EQ(i8, i8a);
    EXPECT_EQ(i16, i16a);
    EXPECT_EQ(i32, i32a);
    EXPECT_EQ(i64, i64a);
}

TEST(BinarySerializer, String) {
    stream::OutputStream stream(1024);

    const std::string str = "hello world";
    ASSERT_TRUE(Serializer<std::string>::serialize(stream, str));

    stream::InputStream in(stream.getBuffer());

    std::string read;
    ASSERT_TRUE(Serializer<std::string>::deserialize(in, read));
    EXPECT_EQ(read, str);
}

TEST(BinarySerializer, Array) {
    stream::OutputStream stream(1024);

    const std::uint16_t arr[3] = {34, 13, 5};
    ASSERT_TRUE(Serializer<decltype(arr)>::serialize(stream, arr));

    stream::InputStream in(stream.getBuffer());

    std::uint16_t read[3];
    ASSERT_TRUE(Serializer<decltype(read)>::deserialize(in, read));
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(BinarySerializer, Vector) {
    stream::OutputStream stream(1024);

    const std::vector<std::uint16_t> arr = {34, 13, 5};
    ASSERT_TRUE(Serializer<std::vector<std::uint16_t>>::serialize(stream, arr));

    stream::InputStream in(stream.getBuffer());

    std::vector<std::uint16_t> read;
    ASSERT_TRUE(Serializer<std::vector<std::uint16_t>>::deserialize(in, read));
    ASSERT_EQ(read.size(), 3);
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(BinarySerializer, Vector2D) {
    stream::OutputStream stream(1024);

    ctr::Vector2D<std::uint8_t> arr;
    arr.setSize(2, 2);
    arr(0, 0) = 5;
    arr(0, 1) = 10;
    arr(1, 0) = 15;
    arr(1, 1) = 25;
    ASSERT_TRUE(Serializer<ctr::Vector2D<std::uint8_t>>::serialize(stream, arr));

    stream::InputStream in(stream.getBuffer());

    ctr::Vector2D<std::uint8_t> read;
    ASSERT_TRUE(Serializer<ctr::Vector2D<std::uint8_t>>::deserialize(in, read));
    ASSERT_EQ(read.getWidth(), 2u);
    ASSERT_EQ(read.getHeight(), 2u);
    EXPECT_EQ(arr(0, 0), read(0, 0));
    EXPECT_EQ(arr(0, 1), read(0, 1));
    EXPECT_EQ(arr(1, 0), read(1, 0));
    EXPECT_EQ(arr(1, 1), read(1, 1));
}

TEST(BinarySerializer, HashMap) {
    stream::OutputStream stream(1024);

    std::unordered_map<std::string, std::string> map;
    map["hello"] = "world";
    map["cat"]   = "house";
    const bool rs =
        Serializer<std::unordered_map<std::string, std::string>>::serialize(stream, map);
    ASSERT_TRUE(rs);

    stream::InputStream in(stream.getBuffer());

    std::unordered_map<std::string, std::string> read;
    const bool rd = Serializer<std::unordered_map<std::string, std::string>>::deserialize(in, read);
    ASSERT_TRUE(rd);
    auto it = read.find("hello");
    ASSERT_NE(it, read.end());
    EXPECT_EQ(it->second, "world");
    it = read.find("cat");
    ASSERT_NE(it, read.end());
    EXPECT_EQ(it->second, "house");
}

TEST(BinarySerializer, HashSet) {
    stream::OutputStream stream(1024);

    const std::unordered_set<std::string> set = {"one", "fish", "two", "foosh"};
    ASSERT_TRUE(Serializer<std::unordered_set<std::string>>::serialize(stream, set));

    stream::InputStream in(stream.getBuffer());

    std::unordered_set<std::string> read;
    ASSERT_TRUE(Serializer<std::unordered_set<std::string>>::deserialize(in, read));
    EXPECT_NE(read.find("one"), read.end());
    EXPECT_NE(read.find("fish"), read.end());
    EXPECT_NE(read.find("two"), read.end());
    EXPECT_NE(read.find("foosh"), read.end());
}

TEST(BinarySerializer, SFML) {
    stream::OutputStream stream(1024);

    const sf::Vector2i v2i(54, -108);
    const sf::Vector2u v2u(343, 234234);
    const sf::IntRect rect({32, 56}, {9, -12});
    ASSERT_TRUE(Serializer<sf::Vector2i>::serialize(stream, v2i));
    ASSERT_TRUE(Serializer<sf::Vector2u>::serialize(stream, v2u));
    ASSERT_TRUE(Serializer<sf::IntRect>::serialize(stream, rect));

    stream::InputStream in(stream.getBuffer());

    sf::Vector2i r2i;
    sf::Vector2u r2u;
    sf::IntRect rrect;
    ASSERT_TRUE(Serializer<sf::Vector2i>::deserialize(in, r2i));
    ASSERT_TRUE(Serializer<sf::Vector2u>::deserialize(in, r2u));
    ASSERT_TRUE(Serializer<sf::IntRect>::deserialize(in, rrect));
    EXPECT_EQ(r2i, v2i);
    EXPECT_EQ(r2u, v2u);
    EXPECT_EQ(rect, rrect);
}

TEST(BinarySerializer, PairVariant) {
    stream::OutputStream stream(1024);

    const std::pair<std::uint8_t, std::int32_t> pair(34, -2342);
    const std::variant<std::string, std::uint8_t> variant("fish");
    const bool rp = Serializer<std::pair<std::uint8_t, std::int32_t>>::serialize(stream, pair);
    ASSERT_TRUE(rp);
    const bool rv = Serializer<std::variant<std::string, std::uint8_t>>::serialize(stream, variant);
    ASSERT_TRUE(rv);

    stream::InputStream in(stream.getBuffer());

    std::pair<std::uint8_t, std::int32_t> rpair;
    std::variant<std::string, std::uint8_t> rvar;
    const bool rrp = Serializer<std::pair<std::uint8_t, std::int32_t>>::deserialize(in, rpair);
    ASSERT_TRUE(rrp);
    const bool rrv = Serializer<std::variant<std::string, std::uint8_t>>::deserialize(in, rvar);
    ASSERT_TRUE(rrv);
    EXPECT_EQ(rpair.first, pair.first);
    EXPECT_EQ(rpair.second, pair.second);
    ASSERT_EQ(variant.index(), rvar.index());
    EXPECT_EQ(std::get<std::string>(rvar), "fish");
}

TEST(BinarySerializer, Bool) {
    stream::OutputStream stream(1024);

    const bool t = true;
    const bool f = false;
    ASSERT_TRUE(Serializer<bool>::serialize(stream, t));
    ASSERT_TRUE(Serializer<bool>::serialize(stream, f));

    stream::InputStream in(stream.getBuffer());

    bool rt = false;
    bool rf = true;
    ASSERT_TRUE(Serializer<bool>::deserialize(in, rt));
    ASSERT_TRUE(Serializer<bool>::deserialize(in, rf));
    EXPECT_EQ(rt, t);
    EXPECT_EQ(rf, f);
}

TEST(BinarySerializer, Float) {
    stream::OutputStream stream(1024);

    const float value = 3.14f;
    ASSERT_TRUE(Serializer<float>::serialize(stream, value));

    stream::InputStream in(stream.getBuffer());

    float read = 0.f;
    ASSERT_TRUE(Serializer<float>::deserialize(in, read));
    EXPECT_NEAR(read, value, 0.0001f);
}

TEST(BinarySerializer, Double) {
    stream::OutputStream stream(1024);

    const double value = 2.71828;
    ASSERT_TRUE(Serializer<double>::serialize(stream, value));

    stream::InputStream in(stream.getBuffer());

    double read = 0.0;
    ASSERT_TRUE(Serializer<double>::deserialize(in, read));
    EXPECT_NEAR(read, value, 0.0001);
}

TEST(BinarySerializer, StdArray) {
    stream::OutputStream stream(1024);

    const std::array<std::uint32_t, 3> arr = {100, 200, 300};
    ASSERT_TRUE((Serializer<std::array<std::uint32_t, 3>>::serialize(stream, arr)));

    stream::InputStream in(stream.getBuffer());

    std::array<std::uint32_t, 3> read = {};
    ASSERT_TRUE((Serializer<std::array<std::uint32_t, 3>>::deserialize(in, read)));
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(BinarySerializer, Enum) {
    enum class Color : std::uint8_t { Red = 1, Green = 2, Blue = 3 };

    stream::OutputStream stream(1024);

    const Color value = Color::Green;
    ASSERT_TRUE(Serializer<Color>::serialize(stream, value));

    stream::InputStream in(stream.getBuffer());

    Color read = Color::Red;
    ASSERT_TRUE(Serializer<Color>::deserialize(in, read));
    EXPECT_EQ(read, value);
}

TEST(BinarySerializer, SFMLVector3) {
    stream::OutputStream stream(1024);

    const sf::Vector3f v(1.5f, 2.5f, 3.5f);
    ASSERT_TRUE(Serializer<sf::Vector3f>::serialize(stream, v));

    stream::InputStream in(stream.getBuffer());

    sf::Vector3f read;
    ASSERT_TRUE(Serializer<sf::Vector3f>::deserialize(in, read));
    EXPECT_NEAR(read.x, v.x, 0.0001f);
    EXPECT_NEAR(read.y, v.y, 0.0001f);
    EXPECT_NEAR(read.z, v.z, 0.0001f);
}

TEST(BinarySerializer, GlmVec2) {
    stream::OutputStream stream(1024);

    const glm::vec2 v(1.0f, 2.0f);
    ASSERT_TRUE((Serializer<glm::vec2>::serialize(stream, v)));

    stream::InputStream in(stream.getBuffer());

    glm::vec2 read(0.f);
    ASSERT_TRUE((Serializer<glm::vec2>::deserialize(in, read)));
    EXPECT_NEAR(read.x, v.x, 0.0001f);
    EXPECT_NEAR(read.y, v.y, 0.0001f);
}

TEST(BinarySerializer, GlmVec3) {
    stream::OutputStream stream(1024);

    const glm::vec3 v(1.0f, 2.0f, 3.0f);
    ASSERT_TRUE((Serializer<glm::vec3>::serialize(stream, v)));

    stream::InputStream in(stream.getBuffer());

    glm::vec3 read(0.f);
    ASSERT_TRUE((Serializer<glm::vec3>::deserialize(in, read)));
    EXPECT_NEAR(read.x, v.x, 0.0001f);
    EXPECT_NEAR(read.y, v.y, 0.0001f);
    EXPECT_NEAR(read.z, v.z, 0.0001f);
}

TEST(BinarySerializer, GlmVec4) {
    stream::OutputStream stream(1024);

    const glm::vec4 v(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_TRUE((Serializer<glm::vec4>::serialize(stream, v)));

    stream::InputStream in(stream.getBuffer());

    glm::vec4 read(0.f);
    ASSERT_TRUE((Serializer<glm::vec4>::deserialize(in, read)));
    EXPECT_NEAR(read.x, v.x, 0.0001f);
    EXPECT_NEAR(read.y, v.y, 0.0001f);
    EXPECT_NEAR(read.z, v.z, 0.0001f);
    EXPECT_NEAR(read.w, v.w, 0.0001f);
}

TEST(BinarySerializer, GlmQuat) {
    stream::OutputStream stream(1024);

    const glm::quat q(1.0f, 0.5f, 0.25f, 0.1f);
    ASSERT_TRUE((Serializer<glm::quat>::serialize(stream, q)));

    stream::InputStream in(stream.getBuffer());

    glm::quat read(0.f, 0.f, 0.f, 0.f);
    ASSERT_TRUE((Serializer<glm::quat>::deserialize(in, read)));
    EXPECT_NEAR(read.x, q.x, 0.0001f);
    EXPECT_NEAR(read.y, q.y, 0.0001f);
    EXPECT_NEAR(read.z, q.z, 0.0001f);
    EXPECT_NEAR(read.w, q.w, 0.0001f);
}

TEST(BinarySerializer, GlmMat4) {
    stream::OutputStream stream(1024);

    glm::mat4 m(0.f);
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) { m[c][r] = static_cast<float>(c * 4 + r); }
    }
    ASSERT_TRUE((Serializer<glm::mat4>::serialize(stream, m)));

    stream::InputStream in(stream.getBuffer());

    glm::mat4 read(0.f);
    ASSERT_TRUE((Serializer<glm::mat4>::deserialize(in, read)));
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) { EXPECT_NEAR(read[c][r], m[c][r], 0.0001f); }
    }
}

TEST(BinarySerializer, Optional) {
    stream::OutputStream stream(1024);

    const std::optional<std::uint32_t> withValue(42u);
    const std::optional<std::uint32_t> noValue;
    ASSERT_TRUE((Serializer<std::optional<std::uint32_t>>::serialize(stream, withValue)));
    ASSERT_TRUE((Serializer<std::optional<std::uint32_t>>::serialize(stream, noValue)));

    stream::InputStream in(stream.getBuffer());

    std::optional<std::uint32_t> rWith;
    std::optional<std::uint32_t> rNone;
    ASSERT_TRUE((Serializer<std::optional<std::uint32_t>>::deserialize(in, rWith)));
    ASSERT_TRUE((Serializer<std::optional<std::uint32_t>>::deserialize(in, rNone)));
    ASSERT_TRUE(rWith.has_value());
    EXPECT_EQ(*rWith, *withValue);
    EXPECT_FALSE(rNone.has_value());
}

TEST(BinarySerializer, SFMLImage) {
    stream::OutputStream stream(4096);

    sf::Image image(sf::Vector2u{2, 2});
    image.setPixel({0, 0}, sf::Color(255, 0, 0, 255));
    image.setPixel({1, 0}, sf::Color(0, 255, 0, 255));
    image.setPixel({0, 1}, sf::Color(0, 0, 255, 255));
    image.setPixel({1, 1}, sf::Color(255, 255, 0, 255));
    ASSERT_TRUE(Serializer<sf::Image>::serialize(stream, image));

    stream::InputStream in(stream.getBuffer());

    sf::Image read;
    ASSERT_TRUE(Serializer<sf::Image>::deserialize(in, read));
    ASSERT_EQ(read.getSize(), sf::Vector2u(2, 2));
    EXPECT_EQ(read.getPixel({0, 0}), sf::Color(255, 0, 0, 255));
    EXPECT_EQ(read.getPixel({1, 0}), sf::Color(0, 255, 0, 255));
    EXPECT_EQ(read.getPixel({0, 1}), sf::Color(0, 0, 255, 255));
    EXPECT_EQ(read.getPixel({1, 1}), sf::Color(255, 255, 0, 255));
}

TEST(BinarySerializer, Pointer) {
    stream::OutputStream stream(1024);

    std::uint32_t value = 987u;
    ASSERT_TRUE((Serializer<std::uint32_t*>::serialize(stream, &value)));

    stream::InputStream in(stream.getBuffer());

    std::uint32_t read = 0u;
    ASSERT_TRUE((Serializer<std::uint32_t*>::deserialize(in, &read)));
    EXPECT_EQ(read, value);
}

} // namespace unittest
} // namespace binary
} // namespace serial
} // namespace bl
