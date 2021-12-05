#include <BLIB/Serialization.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace serial
{
namespace binary
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

struct DefaultLoader : public VersionedSerializer<Payload> {
    virtual bool read(Payload& result, InputStream& input) const override {
        if (!input.read(result.ogField)) return false;
        if (!input.read(result.ogString)) return false;
        return true;
    }

    virtual bool write(const Payload& value, OutputStream& output) const override {
        if (!output.write(value.ogField)) return false;
        if (!output.write(value.ogString)) return false;
        return true;
    }
};

using Version0 = DefaultLoader;

struct Version1 : public Version0 {
    virtual bool read(Payload& result, InputStream& input) const override {
        if (!Version0::read(result, input)) return false;
        if (!input.read(result.newInt)) return false;
        return true;
    }

    virtual bool write(const Payload& value, OutputStream& output) const override {
        if (!Version0::write(value, output)) return false;
        if (!output.write(value.newInt)) return false;
        return true;
    }
};

struct Version2 : public Version1 {
    virtual bool read(Payload& result, InputStream& input) const override {
        if (!Version1::read(result, input)) return false;
        if (!input.read(result.newerString)) return false;
        return true;
    }

    virtual bool write(const Payload& value, OutputStream& output) const override {
        if (!Version1::write(value, output)) return false;
        if (!output.write(value.newerString)) return false;
        return true;
    }
};

} // namespace

TEST(BinaryVersionedSerializer, DefaultLoader) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedFile<Payload, DefaultLoader> file;
    MemoryOutputBuffer outbuf;
    OutputStream out(outbuf);
    ASSERT_TRUE(file.write(out, orig));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(file.read(in, loaded));

    EXPECT_EQ(loaded.ogField, 42);
    EXPECT_EQ(loaded.ogString, "hello");
    EXPECT_EQ(loaded.newInt, 77);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(BinaryVersionedSerializer, MultipleVersions) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedFile<Payload, DefaultLoader, Version0, Version1, Version2> file;
    MemoryOutputBuffer outbuf;
    OutputStream out(outbuf);
    ASSERT_TRUE(file.write(out, orig));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(file.read(in, loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, orig.newInt);
    EXPECT_EQ(loaded.newerString, orig.newerString);
}

TEST(BinaryVersionedSerializer, LoadOldVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedFile<Payload, DefaultLoader, Version0, Version1> oldfile;
    MemoryOutputBuffer outbuf;
    OutputStream out(outbuf);
    ASSERT_TRUE(oldfile.write(out, orig));

    VersionedFile<Payload, DefaultLoader, Version0, Version1, Version2> newfile;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(newfile.read(in, loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, orig.newInt);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(BinaryVersionedSerializer, LoadNoVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedFile<Payload, DefaultLoader> oldfile;
    MemoryOutputBuffer outbuf;
    OutputStream out(outbuf);
    ASSERT_TRUE(oldfile.write(out, orig));

    VersionedFile<Payload, DefaultLoader, Version0, Version1, Version2> newfile;
    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_TRUE(newfile.read(in, loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, 77);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(BinaryVersionedSerializer, BadVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    VersionedFile<Payload, DefaultLoader, Version0> oldfile;
    VersionedFile<Payload, DefaultLoader, Version0, Version1, Version2> newfile;

    MemoryOutputBuffer outbuf;
    OutputStream out(outbuf);
    ASSERT_TRUE(newfile.write(out, orig));

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    ASSERT_FALSE(oldfile.read(in, loaded));
}

} // namespace unittest
} // namespace binary
} // namespace serial
} // namespace bl
