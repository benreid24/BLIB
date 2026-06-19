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

struct DefaultLoader {
    static bool read(Payload& result, stream::InputStream& input) {
        detail::InputStreamWrapper wrapper(input);
        if (!wrapper.read(result.ogField)) return false;
        if (!wrapper.read(result.ogString)) return false;
        return true;
    }

    static bool write(const Payload& value, stream::OutputStream& output) {
        detail::OutputStreamWrapper wrapper(output);
        if (!wrapper.write(value.ogField)) return false;
        if (!wrapper.write(value.ogString)) return false;
        return true;
    }
};

using Version0 = DefaultLoader;

struct Version1 {
    static bool read(Payload& result, stream::InputStream& input) {
        detail::InputStreamWrapper wrapper(input);
        if (!Version0::read(result, input)) return false;
        if (!wrapper.read(result.newInt)) return false;
        return true;
    }

    static bool write(const Payload& value, stream::OutputStream& output) {
        detail::OutputStreamWrapper wrapper(output);
        if (!Version0::write(value, output)) return false;
        if (!wrapper.write(value.newInt)) return false;
        return true;
    }
};

struct Version2 {
    static bool read(Payload& result, stream::InputStream& input) {
        detail::InputStreamWrapper wrapper(input);
        if (!Version1::read(result, input)) return false;
        if (!wrapper.read(result.newerString)) return false;
        return true;
    }

    static bool write(const Payload& value, stream::OutputStream& output) {
        detail::OutputStreamWrapper wrapper(output);
        if (!Version1::write(value, output)) return false;
        if (!wrapper.write(value.newerString)) return false;
        return true;
    }
};

} // namespace

TEST(BinaryVersionedSerializer, DefaultLoader) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    using TestLoader = VersionedSerializer<Payload, DefaultLoader, DefaultLoader>;
    stream::OutputStream out(1024);
    ASSERT_TRUE(TestLoader::write(out, orig));

    stream::InputStream in(out.getBuffer());
    ASSERT_TRUE(TestLoader::read(in, loaded));

    EXPECT_EQ(loaded.ogField, 42);
    EXPECT_EQ(loaded.ogString, "hello");
    EXPECT_EQ(loaded.newInt, 77);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(BinaryVersionedSerializer, MultipleVersions) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    using TestLoader = VersionedSerializer<Payload, DefaultLoader, Version0, Version1, Version2>;
    stream::OutputStream out(1024);
    ASSERT_TRUE(TestLoader::write(out, orig));

    stream::InputStream in(out.getBuffer());
    ASSERT_TRUE(TestLoader::read(in, loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, orig.newInt);
    EXPECT_EQ(loaded.newerString, orig.newerString);
}

TEST(BinaryVersionedSerializer, LoadOldVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    using OldLoader = VersionedSerializer<Payload, DefaultLoader, Version0, Version1>;
    stream::OutputStream out(1024);
    ASSERT_TRUE(OldLoader::write(out, orig));

    using NewLoader = VersionedSerializer<Payload, DefaultLoader, Version0, Version1, Version2>;
    stream::InputStream in(out.getBuffer());
    ASSERT_TRUE(NewLoader::read(in, loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, orig.newInt);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(BinaryVersionedSerializer, LoadNoVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    using OldLoader = VersionedSerializer<Payload, DefaultLoader, DefaultLoader>;
    stream::OutputStream out(1024);
    ASSERT_TRUE(OldLoader::write(out, orig));

    using NewLoader = VersionedSerializer<Payload, DefaultLoader, Version0, Version1, Version2>;
    stream::InputStream in(out.getBuffer());
    ASSERT_TRUE(NewLoader::read(in, loaded));

    EXPECT_EQ(loaded.ogField, orig.ogField);
    EXPECT_EQ(loaded.ogString, orig.ogString);
    EXPECT_EQ(loaded.newInt, 77);
    EXPECT_EQ(loaded.newerString, "not goodbye");
}

TEST(BinaryVersionedSerializer, BadVersion) {
    const Payload orig = {42, "hello", 55, "goodbye"};
    Payload loaded     = {1234, "oh no", 77, "not goodbye"};

    using OldLoader = VersionedSerializer<Payload, DefaultLoader, Version0>;
    using NewLoader = VersionedSerializer<Payload, DefaultLoader, Version0, Version1, Version2>;

    stream::OutputStream out(1024);
    ASSERT_TRUE(NewLoader::write(out, orig));

    stream::InputStream in(out.getBuffer());
    ASSERT_FALSE(OldLoader::read(in, loaded));
}

TEST(BinaryVersionedSerializer, EnsureVersionOrder) {
    using VS = VersionedSerializer<Payload, DefaultLoader, Version0, Version1, Version2>;

    Payload value = {1234, "oh no", 77, "not goodbye"};
    stream::OutputStream out(1024);
    ASSERT_TRUE(VS::write(out, value));

    stream::InputStream in(out.getBuffer());
    detail::InputStreamWrapper wrapper(in);

    std::uint32_t header;
    ASSERT_TRUE(wrapper.read<std::uint32_t>(header));
    std::uint32_t version;
    ASSERT_TRUE(wrapper.read<std::uint32_t>(version));
    EXPECT_EQ(version, 2);
}

} // namespace unittest
} // namespace binary
} // namespace serial
} // namespace bl
