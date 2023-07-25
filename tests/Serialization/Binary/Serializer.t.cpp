#include <BLIB/Logging.hpp>
#include <BLIB/Serialization/Binary.hpp>
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
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

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

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

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
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    const std::string str = "hello world";
    ASSERT_TRUE(Serializer<std::string>::serialize(stream, str));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

    std::string read;
    ASSERT_TRUE(Serializer<std::string>::deserialize(in, read));
    EXPECT_EQ(read, str);
}

TEST(BinarySerializer, Array) {
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    const std::uint16_t arr[3] = {34, 13, 5};
    ASSERT_TRUE(Serializer<decltype(arr)>::serialize(stream, arr));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

    std::uint16_t read[3];
    ASSERT_TRUE(Serializer<decltype(read)>::deserialize(in, read));
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(BinarySerializer, Vector) {
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    const std::vector<std::uint16_t> arr = {34, 13, 5};
    ASSERT_TRUE(Serializer<std::vector<std::uint16_t>>::serialize(stream, arr));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

    std::vector<std::uint16_t> read;
    ASSERT_TRUE(Serializer<std::vector<std::uint16_t>>::deserialize(in, read));
    ASSERT_EQ(read.size(), 3);
    EXPECT_EQ(read[0], arr[0]);
    EXPECT_EQ(read[1], arr[1]);
    EXPECT_EQ(read[2], arr[2]);
}

TEST(BinarySerializer, Vector2D) {
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    ctr::Vector2D<std::uint8_t> arr;
    arr.setSize(2, 2);
    arr(0, 0) = 5;
    arr(0, 1) = 10;
    arr(1, 0) = 15;
    arr(1, 1) = 25;
    ASSERT_TRUE(Serializer<ctr::Vector2D<std::uint8_t>>::serialize(stream, arr));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

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
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    std::unordered_map<std::string, std::string> map;
    map["hello"] = "world";
    map["cat"]   = "house";
    const bool rs =
        Serializer<std::unordered_map<std::string, std::string>>::serialize(stream, map);
    ASSERT_TRUE(rs);

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

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
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    const std::unordered_set<std::string> set = {"one", "fish", "two", "foosh"};
    ASSERT_TRUE(Serializer<std::unordered_set<std::string>>::serialize(stream, set));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

    std::unordered_set<std::string> read;
    ASSERT_TRUE(Serializer<std::unordered_set<std::string>>::deserialize(in, read));
    EXPECT_NE(read.find("one"), read.end());
    EXPECT_NE(read.find("fish"), read.end());
    EXPECT_NE(read.find("two"), read.end());
    EXPECT_NE(read.find("foosh"), read.end());
}

TEST(BinarySerializer, SFML) {
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    const sf::Vector2i v2i(54, -108);
    const sf::Vector2u v2u(343, 234234);
    const sf::IntRect rect(32, 56, 9, -12);
    ASSERT_TRUE(Serializer<sf::Vector2i>::serialize(stream, v2i));
    ASSERT_TRUE(Serializer<sf::Vector2u>::serialize(stream, v2u));
    ASSERT_TRUE(Serializer<sf::IntRect>::serialize(stream, rect));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

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
    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);

    const std::pair<std::uint8_t, std::int32_t> pair(34, -2342);
    const std::variant<std::string, std::uint8_t> variant("fish");
    const bool rp = Serializer<std::pair<std::uint8_t, std::int32_t>>::serialize(stream, pair);
    ASSERT_TRUE(rp);
    const bool rv = Serializer<std::variant<std::string, std::uint8_t>>::serialize(stream, variant);
    ASSERT_TRUE(rv);

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);

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

} // namespace unittest
} // namespace binary
} // namespace serial
} // namespace bl
