#include <BLIB/Serialization.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace binary
{
class TestyBoi {
public:
    std::string& strF() { return str; }

    std::uint32_t& u32F() { return u32; }

    std::int16_t& nowidthF() { return nowidth; }

    bool& bF() { return b; }

private:
    std::string str;
    std::uint32_t u32;
    std::int16_t nowidth;
    bool b;

    friend class SerializableObject<TestyBoi>;
};

template<>
struct SerializableObject<TestyBoi> : public SerializableObjectBase {
    SerializableField<1, std::string, offsetof(TestyBoi, str)> str;
    SerializableField<2, std::uint32_t, offsetof(TestyBoi, u32)> u32;
    SerializableField<3, std::int16_t, offsetof(TestyBoi, nowidth)> nowidth;
    SerializableField<4, bool, offsetof(TestyBoi, b)> b;

    SerializableObject()
    : str(*this)
    , u32(*this)
    , nowidth(*this)
    , b(*this) {}
};

class TestyBoi2 {
public:
    std::string& strF() { return str; }

    std::uint32_t& u32F() { return u32; }

    std::int16_t& nowidthF() { return nowidth; }

    bool& bF() { return b; }

    std::string& newfieldF() { return newfield; }

private:
    std::string str;
    std::uint32_t u32;
    std::int16_t nowidth;
    bool b;
    std::string newfield;

    friend class SerializableObject<TestyBoi2>;
};

template<>
struct SerializableObject<TestyBoi2> : public SerializableObjectBase {
    SerializableField<1, std::string, offsetof(TestyBoi2, str)> str;
    SerializableField<2, std::uint32_t, offsetof(TestyBoi2, u32)> u32;
    SerializableField<3, std::int16_t, offsetof(TestyBoi2, nowidth)> nowidth;
    SerializableField<4, bool, offsetof(TestyBoi2, b)> b;
    SerializableField<5, std::string, offsetof(TestyBoi2, newfield)> newfield;

    SerializableObject()
    : str(*this)
    , u32(*this)
    , nowidth(*this)
    , b(*this)
    , newfield(*this) {}
};

namespace unittest
{
TEST(BinarySerializableObject, SerializeObject) {
    TestyBoi boi;
    boi.strF()     = "hello world";
    boi.u32F()     = 5634533;
    boi.nowidthF() = -4534;
    boi.bF()       = true;

    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);
    ASSERT_TRUE(Serializer<TestyBoi>::serialize(stream, boi));
    ASSERT_EQ(outbuf.size(), Serializer<TestyBoi>::size(boi));

    TestyBoi read;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(Serializer<TestyBoi>::deserialize(in, read));

    EXPECT_EQ(boi.strF(), read.strF());
    EXPECT_EQ(boi.u32F(), read.u32F());
    EXPECT_EQ(boi.nowidthF(), read.nowidthF());
    EXPECT_EQ(boi.bF(), read.bF());
}

TEST(BinarySerializableObject, NewReadOld) {
    TestyBoi boi;
    boi.strF()     = "hello world";
    boi.u32F()     = 5634533;
    boi.nowidthF() = -4534;
    boi.bF()       = true;

    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);
    ASSERT_TRUE(Serializer<TestyBoi>::serialize(stream, boi));
    ASSERT_EQ(outbuf.size(), Serializer<TestyBoi>::size(boi));

    TestyBoi2 read;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(Serializer<TestyBoi2>::deserialize(in, read));

    EXPECT_EQ(boi.strF(), read.strF());
    EXPECT_EQ(boi.u32F(), read.u32F());
    EXPECT_EQ(boi.nowidthF(), read.nowidthF());
    EXPECT_EQ(boi.bF(), read.bF());
    EXPECT_EQ(read.newfieldF(), "");
}

TEST(BinarySerializableObject, OldReadNew) {
    TestyBoi2 boi;
    boi.strF()      = "hello world";
    boi.u32F()      = 5634533;
    boi.nowidthF()  = -4534;
    boi.bF()        = true;
    boi.newfieldF() = "unread";

    MemoryOutputBuffer outbuf;
    OutputStream stream(outbuf);
    ASSERT_TRUE(Serializer<TestyBoi2>::serialize(stream, boi));
    ASSERT_EQ(outbuf.size(), Serializer<TestyBoi2>::size(boi));

    TestyBoi read;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(Serializer<TestyBoi>::deserialize(in, read));

    EXPECT_EQ(boi.strF(), read.strF());
    EXPECT_EQ(boi.u32F(), read.u32F());
    EXPECT_EQ(boi.nowidthF(), read.nowidthF());
    EXPECT_EQ(boi.bF(), read.bF());
}

} // namespace unittest
} // namespace binary
} // namespace serial
} // namespace bl