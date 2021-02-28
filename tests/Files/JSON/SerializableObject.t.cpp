#include <BLIB/Files/JSON.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace json
{
namespace unittest
{
namespace
{
struct Nested : public SerializableObject {
    Nested()
    : boolValue("bool", *this)
    , floatValue("float", *this) {}

    Nested(bool bval, float fval)
    : Nested() {
        boolValue.setValue(bval);
        floatValue.setValue(fval);
    }

    Nested(const Nested& copy) : Nested() {
        boolValue.setValue(copy.boolValue.getValue());
        floatValue.setValue(copy.floatValue.getValue());
    }

    SerializableField<bool> boolValue;
    SerializableField<float> floatValue;
};

struct Data : public SerializableObject {
    Data(int ival, const std::string sval, const std::vector<Nested>& nval)
    : intValue("int", *this)
    , stringValue("string", *this)
    , nestedValue("nested", *this) {
        intValue.setValue(ival);
        stringValue.setValue(sval);
        nestedValue.setValue(nval);
    }

    SerializableField<int> intValue;
    SerializableField<std::string> stringValue;
    SerializableField<std::vector<Nested>> nestedValue;
};

} // namespace

TEST(JsonSerializableObject, SingleLevel) {
    Nested orig(true, 37.5);
    const Group encoded = orig.serialize();

    Nested unpacked(false, -5);
    ASSERT_TRUE(unpacked.deserialize(encoded));
    EXPECT_EQ(unpacked.boolValue.getValue(), true);
    EXPECT_NEAR(unpacked.floatValue.getValue(), 37.5, 0.1);
}

TEST(JsonSerializableObject, Nested) {
    Data data(42, "string", {Nested(true, 37.5), Nested(false, 11)});
    const Group encoded = data.serialize();

    Data unpacked(343, "notstring", {Nested(false, 101)});
    ASSERT_TRUE(unpacked.deserialize(encoded));
    EXPECT_EQ(unpacked.intValue.getValue(), 42);
    EXPECT_EQ(unpacked.stringValue.getValue(), "string");
    EXPECT_EQ(unpacked.nestedValue.getValue()[0].boolValue.getValue(), true);
    EXPECT_NEAR(unpacked.nestedValue.getValue()[0].floatValue.getValue(), 37.5, 0.1);
    EXPECT_EQ(unpacked.nestedValue.getValue()[1].boolValue.getValue(), false);
    EXPECT_NEAR(unpacked.nestedValue.getValue()[1].floatValue.getValue(), 11, 0.1);
}

} // namespace unittest
} // namespace json
} // namespace bl
