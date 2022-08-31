#include <BLIB/Serialization/JSON.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace json
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
    SerializableField<Nested, bool> boolValue;
    SerializableField<Nested, float> floatValue;

    SerializableObject()
    : SerializableObjectBase(SerializableObjectBase::StrictMode{})
    , boolValue("bval", *this, &Nested::boolValue)
    , floatValue("fval", *this, &Nested::floatValue) {}
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
    SerializableField<Data, int> intValue;
    SerializableField<Data, std::string> stringValue;
    SerializableField<Data, std::vector<Nested>> nestedValue;

    SerializableObject()
    : SerializableObjectBase(SerializableObjectBase::StrictMode{})
    , intValue("ival", *this, &Data::intValue)
    , stringValue("sval", *this, &Data::stringValue)
    , nestedValue("nval", *this, &Data::nestedValue) {}
};

struct Relaxed {
    int one;
    int two;
    int three;
};

template<>
struct SerializableObject<Relaxed> : SerializableObjectBase {
    SerializableField<Relaxed, int> one;
    SerializableField<Relaxed, int, true> two;
    SerializableField<Relaxed, int> three;

    SerializableObject()
    : SerializableObjectBase(SerializableObjectBase::RelaxedMode{})
    , one("one", *this, &Relaxed::one)
    , two("two", *this, &Relaxed::two)
    , three("three", *this, &Relaxed::three) {
        two.setDefault(56);
    }
};

namespace unittest
{
namespace
{
using NestedSerializer  = Serializer<Nested>;
using DataSerializer    = Serializer<Data>;
using RelazedSerializer = Serializer<Relaxed>;
} // namespace

TEST(JsonSerializableObject, SingleLevel) {
    Nested orig(true, 37.5);
    const Value encoded = NestedSerializer::serialize(orig);

    Nested unpacked(false, -5);
    ASSERT_TRUE(NestedSerializer::deserialize(unpacked, encoded));
    EXPECT_EQ(unpacked.boolValue, true);
    EXPECT_NEAR(unpacked.floatValue, 37.5, 0.1);
}

TEST(JsonSerializableObject, Nested) {
    Data data(42, "string", {Nested(true, 37.5), Nested(false, 11)});
    const Value encoded = DataSerializer::serialize(data);

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
    const auto data = json::loadFromString("{\"one\": 5, \"three\": 6}");
    ASSERT_EQ(data.getInteger("one"), 5);
    Relaxed result;
    EXPECT_TRUE(RelazedSerializer::deserialize(result, data));
    EXPECT_EQ(result.one, 5);
    EXPECT_EQ(result.two, 56);
    EXPECT_EQ(result.three, 6);
}

TEST(JsonSerializableObject, RequiredField) {
    const auto data = json::loadFromString("{\"one\": 5}");
    Relaxed result;
    ASSERT_FALSE(RelazedSerializer::deserialize(result, data));
}

} // namespace unittest
} // namespace json
} // namespace serial
} // namespace bl
