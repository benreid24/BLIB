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
TEST(BinaryStreams, IntegerEncodeDecode) {
    MemoryOutputBuffer outbuf;
    outbuf.reserve(sizeof(std::int32_t) + sizeof(std::uint8_t) + sizeof(std::int16_t) +
                   sizeof(std::uint64_t));
    OutputStream os(outbuf);

    ASSERT_TRUE(os.write<std::uint8_t>(8));
    ASSERT_TRUE(os.write<std::int16_t>(-2342));
    ASSERT_TRUE(os.write<std::int32_t>(-123456));
    ASSERT_TRUE(os.write<std::uint64_t>(734673743));
    ASSERT_TRUE(os.good());

    MemoryInputBuffer inbuf(outbuf.data(), outbuf.size());
    InputStream in(inbuf);
    std::uint8_t u8;
    std::int16_t i16;
    std::int32_t i32;
    std::uint64_t u64;

    ASSERT_TRUE(in.read<std::uint8_t>(u8));
    ASSERT_TRUE(in.read<std::int16_t>(i16));
    ASSERT_TRUE(in.read<std::int32_t>(i32));
    ASSERT_TRUE(in.read<std::uint64_t>(u64));

    EXPECT_EQ(u8, 8);
    EXPECT_EQ(i16, -2342);
    EXPECT_EQ(i32, -123456);
    EXPECT_EQ(u64, 734673743);
}

TEST(BinaryStreams, Strings) {
    {
        OutputFile of("temp.bin");
        ASSERT_TRUE(of.write("hello world"));
    }

    InputFile in("temp.bin");
    std::string str;
    ASSERT_TRUE(in.read(str));
    EXPECT_EQ(str, "hello world");
}

} // namespace unittest
} // namespace binary
} // namespace serial
} // namespace bl