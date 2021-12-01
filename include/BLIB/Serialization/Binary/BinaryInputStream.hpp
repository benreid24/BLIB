#ifndef BLIB_SERIALIZATION_BINARYINPUTSTREAM_HPP
#define BLIB_SERIALIZATION_BINARYINPUTSTREAM_HPP

#include <BLIB/Files/Util.hpp>
#include <BLIB/Serialization/Buffers/InputBuffer.hpp>

namespace bl
{
/// Collection of classes and utilities for serializing and deserializing data
namespace serial
{
/**
 * @brief A readable stream for binary deserialization
 *
 * @ingroup Serialization
 *
 */
class BinaryInputStream {
public:
    /**
     * @brief Construct a new Binary Input Stream
     *
     * @param underlying The underlying buffer to read from
     */
    BinaryInputStream(InputBuffer& underlying);

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
    InputBuffer& underlying;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

inline BinaryInputStream::BinaryInputStream(InputBuffer& u)
: underlying(u) {}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type BinaryInputStream::read(T& output) {
    if (!underlying.good()) return false;

    const std::size_t size = sizeof(output);
    char* bytes            = static_cast<char*>(static_cast<void*>(&output));

    output       = 0;
    const bool r = underlying.read(bytes, size);
    if (file::Util::isBigEndian()) {
        for (unsigned int i = 0; i < size / 2; ++i) { std::swap(bytes[i], bytes[size - i - 1]); }
    }
    return r;
}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type BinaryInputStream::peek(T& output) {
    const std::size_t s = sizeof(T);
    const bool r        = read<T>(output);
    underlying.seekg(underlying.tellg() - s);
    return r;
}

inline bool BinaryInputStream::read(std::string& output) {
    if (!underlying.good()) return false;
    std::uint32_t size;
    if (!read<std::uint32_t>(size)) return false;
    output.clear();
    output.resize(size);
    return underlying.read(output.data(), size);
}

inline bool BinaryInputStream::skip(std::size_t bytes) {
    return underlying.seekg(underlying.tellg() + bytes);
}

inline bool BinaryInputStream::good() {
    const bool eof = underlying.peek() == EOF;
    return underlying.good() && !eof;
}

} // namespace serial
} // namespace bl

#endif
