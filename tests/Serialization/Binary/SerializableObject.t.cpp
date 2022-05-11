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

    float& fF() { return f; }

private:
    std::string str;
    std::uint32_t u32;
    std::int16_t nowidth;
    bool b;
    float f;

    friend class SerializableObject<TestyBoi>;
};

template<>
struct SerializableObject<TestyBoi> : public SerializableObjectBase {
    SerializableField<1, TestyBoi, std::string> str;
    SerializableField<2, TestyBoi, std::uint32_t> u32;
    SerializableField<3, TestyBoi, std::int16_t> nowidth;
    SerializableField<4, TestyBoi, bool> b;
    SerializableFloatField<5, TestyBoi> f;

    SerializableObject()
    : str(*this, &TestyBoi::str)
    , u32(*this, &TestyBoi::u32)
    , nowidth(*this, &TestyBoi::nowidth)
    , b(*this, &TestyBoi::b)
    , f(*this, &TestyBoi::f, 100.f) {}
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
    SerializableField<1, TestyBoi2, std::string> str;
    SerializableField<2, TestyBoi2, std::uint32_t> u32;
    SerializableField<3, TestyBoi2, std::int16_t> nowidth;
    SerializableField<4, TestyBoi2, bool> b;
    SerializableField<6, TestyBoi2, std::string> newfield;

    SerializableObject()
    : str(*this, &TestyBoi2::str)
    , u32(*this, &TestyBoi2::u32)
    , nowidth(*this, &TestyBoi2::nowidth)
    , b(*this, &TestyBoi2::b)
    , newfield(*this, &TestyBoi2::newfield) {}
};

namespace unittest
{
TEST(BinarySerializableObject, SerializeObject) {
    TestyBoi boi;
    boi.strF()     = "hello world";
    boi.u32F()     = 5634533;
    boi.nowidthF() = -4534;
    boi.bF()       = true;
    boi.fF()       = 0.55f;

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
    EXPECT_FLOAT_EQ(boi.fF(), 0.55f);
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
