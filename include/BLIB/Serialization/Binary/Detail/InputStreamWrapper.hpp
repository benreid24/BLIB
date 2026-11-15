#ifndef BLIB_SERIALIZATION_INPUTSTREAM_HPP
#define BLIB_SERIALIZATION_INPUTSTREAM_HPP

#include <BLIB/Serialization/SerializationError.hpp>
#include <BLIB/Streams/InputStream.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <cstdint>

/**
 * @addtogroup Binary
 * @ingroup Serialization
 * @brief Collection of classes and utilities for serializing data to a binary format
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
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, void>::type read(T& output);

    /**
     * @brief Peeks the integral type from the underlying without moving the cursor
     *
     * @tparam T The type to read. Be explicit with bit width and sign
     * @param output Variable to write the read value to
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, void>::type peek(T& output);

    /**
     * @brief Reads a string from the underlying
     *
     * @param output Variable to write the read string to
     * @return bool True if the string could be read
     */
    void read(std::string& output);

    /**
     * @brief Skips the given number of bytes
     *
     * @param bytes The number of bytes to skip
     * @return True if the underlying underlying is still valid, false otherwise
     */
    void skip(std::size_t bytes);

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
typename std::enable_if<std::is_integral_v<T>, void>::type InputStreamWrapper::read(T& output) {
    if (!underlying.isValid()) { throw SerializationError("Cannot read from invalid stream"); }

    constexpr std::size_t size = sizeof(T);
    char* bytes                = static_cast<char*>(static_cast<void*>(&output));

    output = 0;
    if (underlying.read(bytes, size) != size) {
        throw SerializationError("Failed to read integral type from stream");
    }
    if constexpr (size > 1) {
        if (util::FileUtil::isBigEndian()) {
            for (unsigned int i = 0; i < size / 2; ++i) {
                std::swap(bytes[i], bytes[size - i - 1]);
            }
        }
    }
}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, void>::type InputStreamWrapper::peek(T& output) {
    const std::size_t s = sizeof(T);
    read<T>(output);
    underlying.seek(underlying.tell() - s);
}

inline void InputStreamWrapper::read(std::string& output) {
    if (!underlying.isValid()) { throw SerializationError("Cannot read from invalid stream"); }
    std::uint32_t size;
    read<std::uint32_t>(size);
    output.clear();
    output.resize(size);
    if (underlying.read(output.data(), size) != size) {
        throw SerializationError("Failed to read string from stream");
    }
}

inline void InputStreamWrapper::skip(std::size_t bytes) {
    const std::size_t pos = underlying.tell() + bytes;
    if (pos != underlying.seek(pos)) { throw SerializationError("Failed to skip bytes in stream"); }
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
