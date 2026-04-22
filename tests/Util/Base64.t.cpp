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

} // namespace unittest
} // namespace util
} // namespace bl
