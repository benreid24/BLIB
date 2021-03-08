#include <BLIB/Files/Binary/SerializableField.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace file
{
namespace binary
{
namespace unittest
{
TEST(BinarySerializableField, IntegralTypes) {
    SerializableObject owner;
    SerializableField<1, std::uint32_t> f1(owner);
    SerializableField<2, std::int16_t> f2(owner);
    SerializableField<3, bool> f3(owner);

    f1.setValue(346345);
    f2.setValue(-1234);
    f3.setValue(true);

    {
        File output("inttypes.bin", File::Write);
        ASSERT_TRUE(f1.serialize(output));
        ASSERT_TRUE(f2.serialize(output));
        ASSERT_TRUE(f3.serialize(output));
    }

    SerializableField<1, std::uint32_t> r1(owner);
    SerializableField<2, std::int16_t> r2(owner);
    SerializableField<3, bool> r3(owner);

    {
        File input("inttypes.bin", File::Read);
        ASSERT_TRUE(r1.deserialize(input));
        ASSERT_TRUE(r2.deserialize(input));
        ASSERT_TRUE(r3.deserialize(input));
    }

    EXPECT_EQ(r1.getValue(), f1.getValue());
    EXPECT_EQ(r2.getValue(), f2.getValue());
    EXPECT_EQ(r3.getValue(), f3.getValue());
}

TEST(BinarySerializableField, String) {
    SerializableObject owner;
    SerializableField<1, std::string> field(owner);
    field.setValue("hello world");

    {
        File output("stringtest.bin", File::Write);
        ASSERT_TRUE(field.serialize(output));
    }

    SerializableField<1, std::string> loaded(owner);
    {
        File input("stringtest.bin", File::Read);
        ASSERT_TRUE(loaded.deserialize(input));
    }

    EXPECT_EQ(field.getValue(), loaded.getValue());
}

TEST(BinarySerializableField, IntVector) {
    SerializableObject owner;
    SerializableField<1, std::vector<std::int16_t>> field(owner);
    field.setValue({5, 3, 6, 435});

    {
        File output("stringtest.bin", File::Write);
        ASSERT_TRUE(field.serialize(output));
    }

    SerializableField<1, std::vector<std::int16_t>> loaded(owner);
    {
        File input("stringtest.bin", File::Read);
        ASSERT_TRUE(loaded.deserialize(input));
    }

    ASSERT_EQ(field.getValue().size(), loaded.getValue().size());
    for (unsigned int i = 0; i < loaded.getValue().size(); ++i) {
        EXPECT_EQ(field.getValue().at(i), loaded.getValue().at(i));
    }
}

TEST(BinarySerializableField, StringVector) {
    SerializableObject owner;
    SerializableField<1, std::vector<std::string>> field(owner);
    field.setValue({"hello", "everyone"});

    {
        File output("stringtest.bin", File::Write);
        ASSERT_TRUE(field.serialize(output));
    }

    SerializableField<1, std::vector<std::string>> loaded(owner);
    {
        File input("stringtest.bin", File::Read);
        ASSERT_TRUE(loaded.deserialize(input));
    }

    ASSERT_EQ(field.getValue().size(), loaded.getValue().size());
    for (unsigned int i = 0; i < loaded.getValue().size(); ++i) {
        EXPECT_EQ(field.getValue().at(i), loaded.getValue().at(i));
    }
}

} // namespace unittest
} // namespace binary
} // namespace file
} // namespace bl
