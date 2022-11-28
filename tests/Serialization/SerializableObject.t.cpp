#include <BLIB/Serialization.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
struct Nested {
    bool boolValue;
    float floatValue;

    Nested() {}

    Nested(bool b, float f)
    : boolValue(b)
    , floatValue(f) {}
};

template<>
struct SerializableObject<Nested> : public SerializableObjectBase {
    SerializableField<1, Nested, bool> boolValue;
    SerializableField<2, Nested, float> floatValue;

    SerializableObject()
    : boolValue("bval", *this, &Nested::boolValue, SerializableFieldBase::Required{})
    , floatValue("fval", *this, &Nested::floatValue, SerializableFieldBase::Required{}) {}
};

struct Data {
    Data(int ival, const std::string sval, const std::vector<Nested>& nval)
    : intValue(ival)
    , stringValue(sval)
    , nestedValue(nval) {}

    int intValue;
    std::string stringValue;
    std::vector<Nested> nestedValue;
};

template<>
struct SerializableObject<Data> : public SerializableObjectBase {
    SerializableField<1, Data, int> intValue;
    SerializableField<2, Data, std::string> stringValue;
    SerializableField<3, Data, std::vector<Nested>> nestedValue;

    SerializableObject()
    : intValue("ival", *this, &Data::intValue, SerializableFieldBase::Required{})
    , stringValue("sval", *this, &Data::stringValue, SerializableFieldBase::Required{})
    , nestedValue("nval", *this, &Data::nestedValue, SerializableFieldBase::Required{}) {}
};

struct Relaxed {
    int one;
    int two;
    int three;
};

template<>
struct SerializableObject<Relaxed> : SerializableObjectBase {
    SerializableField<1, Relaxed, int> one;
    SerializableField<2, Relaxed, int> two;
    SerializableField<3, Relaxed, int> three;

    SerializableObject()
    : one("one", *this, &Relaxed::one, SerializableFieldBase::Required{})
    , two("two", *this, &Relaxed::two, SerializableFieldBase::Optional{})
    , three("three", *this, &Relaxed::three, SerializableFieldBase::Required{}) {
        two.setDefault(56);
    }
};

namespace unittest
{
namespace
{
using NestedSerializer  = json::Serializer<Nested>;
using DataSerializer    = json::Serializer<Data>;
using RelazedSerializer = json::Serializer<Relaxed>;
} // namespace

TEST(JsonSerializableObject, SingleLevel) {
    Nested orig(true, 37.5);
    const json::Value encoded = NestedSerializer::serialize(orig);

    Nested unpacked(false, -5);
    ASSERT_TRUE(NestedSerializer::deserialize(unpacked, encoded));
    EXPECT_EQ(unpacked.boolValue, true);
    EXPECT_NEAR(unpacked.floatValue, 37.5, 0.1);
}

TEST(JsonSerializableObject, Nested) {
    Data data(42, "string", {Nested(true, 37.5), Nested(false, 11)});
    const json::Value encoded = DataSerializer::serialize(data);

    Data unpacked(343, "notstring", {Nested(false, 101)});
    ASSERT_TRUE(DataSerializer::deserialize(unpacked, encoded));
    EXPECT_EQ(unpacked.intValue, 42);
    EXPECT_EQ(unpacked.stringValue, "string");
    EXPECT_EQ(unpacked.nestedValue[0].boolValue, true);
    EXPECT_NEAR(unpacked.nestedValue[0].floatValue, 37.5, 0.1);
    EXPECT_EQ(unpacked.nestedValue[1].boolValue, false);
    EXPECT_NEAR(unpacked.nestedValue[1].floatValue, 11, 0.1);
}

TEST(JsonSerializableObject, DefaultValue) {
    std::stringstream input("{\"one\": 5, \"three\": 6}");
    json::Group data;
    ASSERT_TRUE(json::loadFromStream(input, data));
    ASSERT_EQ(data.getInteger("one"), 5);
    Relaxed result;
    EXPECT_TRUE(RelazedSerializer::deserialize(result, data));
    EXPECT_EQ(result.one, 5);
    EXPECT_EQ(result.two, 56);
    EXPECT_EQ(result.three, 6);
}

TEST(JsonSerializableObject, RequiredField) {
    std::stringstream ss("{\"one\": 5}");
    json::Group data;
    ASSERT_TRUE(json::loadFromStream(ss, data));
    Relaxed result;
    ASSERT_FALSE(RelazedSerializer::deserialize(result, data));
}
} // namespace unittest

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
    SerializableField<5, TestyBoi, float> f;

    SerializableObject()
    : str("str", *this, &TestyBoi::str, SerializableFieldBase::Required{})
    , u32("u32", *this, &TestyBoi::u32, SerializableFieldBase::Required{})
    , nowidth("nowidth", *this, &TestyBoi::nowidth, SerializableFieldBase::Required{})
    , b("b", *this, &TestyBoi::b, SerializableFieldBase::Required{})
    , f("f1", *this, &TestyBoi::f, SerializableFieldBase::Optional{}) {}
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

    friend struct SerializableObject<TestyBoi2>;
};

template<>
struct SerializableObject<TestyBoi2> : public SerializableObjectBase {
    SerializableField<1, TestyBoi2, std::string> str;
    SerializableField<2, TestyBoi2, std::uint32_t> u32;
    SerializableField<3, TestyBoi2, std::int16_t> nowidth;
    SerializableField<4, TestyBoi2, bool> b;
    SerializableField<6, TestyBoi2, std::string> newfield;

    SerializableObject()
    : str("str", *this, &TestyBoi2::str, SerializableFieldBase::Required{})
    , u32("u32", *this, &TestyBoi2::u32, SerializableFieldBase::Required{})
    , nowidth("nowidth", *this, &TestyBoi2::nowidth, SerializableFieldBase::Required{})
    , b("b", *this, &TestyBoi2::b, SerializableFieldBase::Required{})
    , newfield("f", *this, &TestyBoi2::newfield, SerializableFieldBase::Optional{}) {}
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
    binary::OutputStream stream(outbuf);
    ASSERT_TRUE(binary::Serializer<TestyBoi>::serialize(stream, boi));
    ASSERT_EQ(outbuf.size(), binary::Serializer<TestyBoi>::size(boi));

    TestyBoi read;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    binary::InputStream in(inbuf);
    ASSERT_TRUE(binary::Serializer<TestyBoi>::deserialize(in, read));

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
    binary::OutputStream stream(outbuf);
    ASSERT_TRUE(binary::Serializer<TestyBoi>::serialize(stream, boi));
    ASSERT_EQ(outbuf.size(), binary::Serializer<TestyBoi>::size(boi));

    TestyBoi2 read;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    binary::InputStream in(inbuf);
    ASSERT_TRUE(binary::Serializer<TestyBoi2>::deserialize(in, read));

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
    binary::OutputStream stream(outbuf);
    ASSERT_TRUE(binary::Serializer<TestyBoi2>::serialize(stream, boi));
    ASSERT_EQ(outbuf.size(), binary::Serializer<TestyBoi2>::size(boi));

    TestyBoi read;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    binary::InputStream in(inbuf);
    ASSERT_TRUE(binary::Serializer<TestyBoi>::deserialize(in, read));

    EXPECT_EQ(boi.strF(), read.strF());
    EXPECT_EQ(boi.u32F(), read.u32F());
    EXPECT_EQ(boi.nowidthF(), read.nowidthF());
    EXPECT_EQ(boi.bF(), read.bF());
}

} // namespace unittest
} // namespace serial
} // namespace bl
