#ifndef BLIB_UTIL_BUFFERISTREAM_HPP
#define BLIB_UTIL_BUFFERISTREAM_HPP

#include <istream>
#include <vector>

namespace bl
{
namespace util
{
/**
 * @brief Helper class that represents a streambuf around a vector<char>. Use to create istream
 *        objects from vector<char> buffers
 *
 * @ingroup Util
 *
 */
struct BufferIstreamBuf : public std::basic_streambuf<char, std::char_traits<char>> {
    /**
     * @brief Construct a new Buffer Istream Buf from the given buffer
     *
     * @param buffer The buffer to wrap
     * @param length The size of the buffer to wrap
     */
    BufferIstreamBuf(char* buffer, std::size_t length) { setg(buffer, buffer, buffer + length); }
};

} // namespace util
} // namespace bl

#endif