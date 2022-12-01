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
     */
    BufferIstreamBuf(std::vector<char>& buffer) {
        setg(buffer.data(), buffer.data(), buffer.data() + buffer.size());
    }
};

} // namespace util
} // namespace bl

#endif