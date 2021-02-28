#include <BLIB/Files/Binary.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace bf
{
namespace unittest
{
namespace
{
struct Nested : public SerializableObject {
    SerializableField<std::int16_t> ifield;
    SerializableField<std::string> sfield;

    Nested()
    : ifield(*this)
    , sfield(*this) {}

    Nested(std::int16_t i, const std::string& s)
    : Nested() {
        ifield.setValue(i);
        sfield.setValue(s);
    }

    Nested(const Nested& copy)
    : Nested() {
        ifield.setValue(copy.ifield.getValue());
        sfield.setValue(copy.sfield.getValue());
    }

    bool operator==(const Nested& right) const {
        if (ifield.getValue() != right.ifield.getValue()) return false;
        return sfield.getValue() == right.sfield.getValue();
    }
};

struct Data : public SerializableObject {
    SerializableField<std::vector<Nested>> nested;

    Data()
    : nested(*this) {}

    Data(const std::vector<Nested>& v)
    : Data() {
        nested.setValue(v);
    }
};

} // namespace

TEST(BinarySerializableObject, SingleLevel) {
    Nested original(52, "test object");

    {
        BinaryFile output("binaryobjectsinglelevel.bin", BinaryFile::Write);
        ASSERT_TRUE(original.serialize(output));
    }

    Nested loaded;
    {
        BinaryFile input("binaryobjectsinglelevel.bin", BinaryFile::Read);
        ASSERT_TRUE(loaded.deserialize(input));
    }

    EXPECT_EQ(original.ifield.getValue(), loaded.ifield.getValue());
    EXPECT_EQ(original.sfield.getValue(), loaded.sfield.getValue());
}

TEST(BinarySerializableObject, NestedObjects) {
    Data data({Nested(35, "nested 1"), Nested(923, "nested two")});

    {
        BinaryFile output("binaryobjectnested.bin", BinaryFile::Write);
        ASSERT_TRUE(data.serialize(output));
    }

    Data loaded;
    {
        BinaryFile input("binaryobjectnested.bin", BinaryFile::Read);
        ASSERT_TRUE(loaded.deserialize(input));
    }

    ASSERT_EQ(data.nested.getValue().size(), loaded.nested.getValue().size());
    for (unsigned int i = 0; i < data.nested.getValue().size(); ++i) {
        EXPECT_EQ(data.nested.getValue().at(i), loaded.nested.getValue().at(i));
    }
}

} // namespace unittest
} // namespace bf
} // namespace bl
