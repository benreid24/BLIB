#ifndef BLIB_SERIALIZATION_INPUTSTREAM_HPP
#define BLIB_SERIALIZATION_INPUTSTREAM_HPP

#include <BLIB/Streams/InputStream.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <cstdint>

/**
 * @addtogroup Binary
 * @ingroup Serialization
 * @brief Collection of classes and utilities for serializing data to a binary format
 *
 */

namespace bl
{
/// Collection of classes and utilities for serializing and deserializing data
namespace serial
{
/// Module for serializing and deserializing binary data
namespace binary
{
/// Implementation details for binary serialization
namespace detail
{
/**
 * @brief A readable stream for binary deserialization
 *
 * @ingroup Binary
 *
 */
class InputStreamWrapper {
public:
    /**
     * @brief Construct a new Binary Input Stream
     *
     * @param underlying The underlying buffer to read from
     */
    InputStreamWrapper(stream::InputStream& underlying);

    /**
     * @brief Reads the integral type from the underlying
     *
     * @tparam T The type to read. Be explicit with bit width and sign
     * @param output Variable to write the read value to
     * @return bool True if the value could be read
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type read(T& output);

    /**
     * @brief Peeks the integral type from the underlying without moving the cursor
     *
     * @tparam T The type to read. Be explicit with bit width and sign
     * @param output Variable to write the read value to
     * @return bool True if the value could be read
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type peek(T& output);

    /**
     * @brief Reads a string from the underlying
     *
     * @param output Variable to write the read string to
     * @return bool True if the string could be read
     */
    bool read(std::string& output);

    /**
     * @brief Skips the given number of bytes
     *
     * @param bytes The number of bytes to skip
     * @return True if the underlying underlying is still valid, false otherwise
     */
    bool skip(std::size_t bytes);

    /**
     * @brief Returns the status of the underlying
     *
     */
    bool good();

private:
    stream::InputStream& underlying;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

inline InputStreamWrapper::InputStreamWrapper(stream::InputStream& u)
: underlying(u) {}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type InputStreamWrapper::read(T& output) {
    if (!underlying.isValid()) return false;

    constexpr std::size_t size = sizeof(T);
    char* bytes                = static_cast<char*>(static_cast<void*>(&output));

    output       = 0;
    const bool r = underlying.read(bytes, size) == size;
    if constexpr (size > 1) {
        if (util::FileUtil::isBigEndian()) {
            for (unsigned int i = 0; i < size / 2; ++i) {
                std::swap(bytes[i], bytes[size - i - 1]);
            }
        }
    }
    return r;
}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type InputStreamWrapper::peek(T& output) {
    const std::size_t s = sizeof(T);
    const bool r        = read<T>(output);
    underlying.seek(underlying.tell() - s);
    return r;
}

inline bool InputStreamWrapper::read(std::string& output) {
    if (!underlying.isValid()) return false;
    std::uint32_t size;
    if (!read<std::uint32_t>(size)) return false;
    output.clear();
    output.resize(size);
    return underlying.read(output.data(), size) == size;
}

inline bool InputStreamWrapper::skip(std::size_t bytes) {
    const std::size_t pos = underlying.tell() + bytes;
    return pos == underlying.seek(pos);
}

inline bool InputStreamWrapper::good() {
    const bool eof = underlying.peek() == EOF;
    return underlying.isValid() && !eof;
}

} // namespace detail
} // namespace binary
} // namespace serial
} // namespace bl

#endif
