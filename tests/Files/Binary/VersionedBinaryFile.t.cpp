#include <BLIB/Files/Binary/VersionedBinaryFile.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace bf
{
namespace unittest
{
namespace
{
struct Payload {
    int ogField;
    std::string ogString;
    int newInt;
    std::string newerString;
};

struct DefaultLoader : public VersionedPayloadLoader<Payload> {
    virtual bool read(Payload& result, BinaryFile& input) const override {
        if (!input.read(result.ogField)) return false;
        if (!input.read(result.ogString)) return false;
        return true;
    }

    virtual bool write(const Payload& value, BinaryFile& output) const override {
        if (!output.write(value.ogField)) return false;
        if (!output.write(value.ogString)) return false;
        return true;
    }
};

using Version0 = DefaultLoader;

struct Version1 : public Version0 {
    virtual bool read(Payload& result, BinaryFile& input) const override {
        if (!Version0::read(result, input)) return false;
        if (!input.read(result.newInt)) return false;
        return true;
    }

    virtual bool write(const Payload& value, BinaryFile& output) const override {
        if (!Version0::write(value, output)) return false;
        if (!output.write(value.newInt)) return false;
        return true;
    }
};

struct Version2 : public Version1 {
    virtual bool read(Payload& result, BinaryFile& input) const override {
        if (!Version1::read(result, input)) return false;
        if (!input.read(result.newerString)) return false;
        return true;
    }

    virtual bool write(const Payload& value, BinaryFile& output) const override {
        if (!Version1::write(value, output)) return false;
        if (!output.write(value.newerString)) return false;
        return true;
    }
};

} // namespace

TEST(VersionedBinaryFile, DefaultLoader) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedBinaryFile<Payload, DefaultLoader> file("vbin_default.bin");
    ASSERT_TRUE(file.write(orig));
    ASSERT_TRUE(file.read(loaded));

    EXPECT_EQ(loaded.ogField, 42);
    EXPECT_EQ(loaded.ogString, "hello");
    EXPECT_EQ(loaded.newInt, 77);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(VersionedBinaryFile, MultipleVersions) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedBinaryFile<Payload, DefaultLoader, Version0, Version1, Version2> file(
        "vbin_default.bin");
    ASSERT_TRUE(file.write(orig));
    ASSERT_TRUE(file.read(loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, orig.newInt);
    EXPECT_EQ(loaded.newerString, orig.newerString);
}

TEST(VersionedBinaryFile, LoadOldVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedBinaryFile<Payload, DefaultLoader, Version0, Version1> oldfile("vbin_default.bin");
    ASSERT_TRUE(oldfile.write(orig));
    VersionedBinaryFile<Payload, DefaultLoader, Version0, Version1, Version2> newfile(
        "vbin_default.bin");
    ASSERT_TRUE(newfile.read(loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, orig.newInt);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(VersionedBinaryFile, LoadNoVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedBinaryFile<Payload, DefaultLoader> oldfile("vbin_default.bin");
    ASSERT_TRUE(oldfile.write(orig));
    VersionedBinaryFile<Payload, DefaultLoader, Version0, Version1, Version2> newfile(
        "vbin_default.bin");
    ASSERT_TRUE(newfile.read(loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, 77);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(VersionedBinaryFile, BadVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedBinaryFile<Payload, DefaultLoader, Version0> oldfile("vbin_default.bin");
    VersionedBinaryFile<Payload, DefaultLoader, Version0, Version1, Version2> newfile(
        "vbin_default.bin");

    ASSERT_TRUE(newfile.write(orig));
    ASSERT_FALSE(oldfile.read(loaded));
}

} // namespace unittest
} // namespace bf
} // namespace bl
