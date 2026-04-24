#include <BLIB/Util/Base64.hpp>
#include <gtest/gtest.h>

namespace bl
{
namespace util
{
namespace unittest
{
TEST(Base64, EncodeDecode) {
    const std::string data    = "Hello, World!";
    const std::string encoded = Base64::encode(data);
    std::vector<char> decoded;
    ASSERT_TRUE(Base64::decode(encoded, decoded));
    ASSERT_EQ(std::string(decoded.begin(), decoded.end()), data);
}

TEST(Base64, HighBytes) {
    const std::vector<char> data = {static_cast<char>(0x89),
                                    static_cast<char>(0xFF),
                                    static_cast<char>(0x80),
                                    static_cast<char>(0xC0),
                                    static_cast<char>(0xAB),
                                    static_cast<char>(0xDE)};
    const std::string encoded    = Base64::encode(std::span<const char>(data));
    std::vector<char> decoded;
    ASSERT_TRUE(Base64::decode(encoded, decoded));
    ASSERT_EQ(decoded.size(), data.size());
    for (std::size_t i = 0; i < data.size(); ++i) { EXPECT_EQ(decoded[i], data[i]); }
}

} // namespace unittest
} // namespace util
} // namespace bl
