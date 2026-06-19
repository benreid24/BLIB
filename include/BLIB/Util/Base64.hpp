#ifndef BLIB_UTIL_BASE64_HPP
#define BLIB_UTIL_BASE64_HPP

#include <span>
#include <string>
#include <vector>

namespace bl
{
namespace util
{
/**
 * @brief Collection of utilities for base64 encoding and decoding
 *
 * @ingroup Util
 */
struct Base64 {
    /**
     * @brief Encodes the given data to a base64 string
     *
     * @param data The data to encode
     * @return The base64 encoded string
     */
    static std::string encode(std::span<const char> data);

    /**
     * @brief Decodes a base64 encoded string
     *
     * @param encoded The base64 encoded string
     * @param result The container to store the decoded bytes
     * @return True if decoding was successful, false otherwise
     */
    static bool decode(const std::string& encoded, std::vector<char>& result);
};

} // namespace util
} // namespace bl

#endif
