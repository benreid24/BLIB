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
    SerializableField<1, std::int16_t> ifield;
    SerializableField<2, std::string> sfield;

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

    Nested& operator=(const Nested& c) {
        ifield.setValue(c.ifield.getValue());
        sfield.setValue(c.sfield.getValue());
        return *this;
    }

    bool operator==(const Nested& right) const {
        if (ifield.getValue() != right.ifield.getValue()) return false;
        return sfield.getValue() == right.sfield.getValue();
    }
};

struct Data : public SerializableObject {
    SerializableField<1, std::vector<Nested>> nested;

    Data()
    : nested(*this) {}

    Data(const std::vector<Nested>& v)
    : Data() {
        nested.setValue(v);
    }
};

struct DataV2 : public SerializableObject {
    SerializableField<1, std::vector<Nested>> nested;
    SerializableField<2, std::string> newString;

    DataV2()
    : nested(*this)
    , newString(*this) {}

    DataV2(const std::vector<Nested>& v, const std::string& s)
    : DataV2() {
        nested.setValue(v);
        newString = s;
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

TEST(BinarySerializableObject, LoadOldVersion) {
    Data old({Nested(35, "nested 1"), Nested(923, "nested two")});

    {
        BinaryFile output("binaryobjectnested.bin", BinaryFile::Write);
        ASSERT_TRUE(old.serialize(output));
    }

    DataV2 loaded;
    loaded.newString = "unchanged";
    {
        BinaryFile input("binaryobjectnested.bin", BinaryFile::Read);
        ASSERT_TRUE(loaded.deserialize(input));
    }

    EXPECT_EQ(loaded.newString.getValue(), "unchanged");
    ASSERT_EQ(old.nested.getValue().size(), loaded.nested.getValue().size());
    for (unsigned int i = 0; i < old.nested.getValue().size(); ++i) {
        EXPECT_EQ(old.nested.getValue().at(i), loaded.nested.getValue().at(i));
    }
}

TEST(BinarySerializableObject, LoadNewVersion) {
    DataV2 data({Nested(35, "nested 1"), Nested(923, "nested two")}, "mystring");
    std::string buffer;

    {
        std::stringstream ss;
        BinaryFile output(ss, BinaryFile::Write);
        ASSERT_TRUE(data.serialize(output));
        buffer = ss.str();
    }

    Data loaded;
    {
        std::stringstream ss(buffer);
        BinaryFile input(ss, BinaryFile::Read);
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
