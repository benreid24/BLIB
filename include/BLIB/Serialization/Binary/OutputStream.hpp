#ifndef BLIB_SERIALIZATION_BINARY_OUTPUTSTREAM_HPP
#define BLIB_SERIALIZATION_BINARY_OUTPUTSTREAM_HPP

#include <BLIB/Serialization/Buffers/OutputBuffer.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <cstring>
#include <string>
#include <type_traits>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief A writable stream for binary serialization
 *
 * @ingroup Binary
 *
 */
class OutputStream {
public:
    /**
     * @brief Construct a new Output Stream
     *
     * @param buffer The underlying buffer to write to
     */
    OutputStream(OutputBuffer& buffer);

    /**
     * @brief Writes an integral type to the stream in it's binary representation
     *
     * @tparam T The type to write. It is better to be explicit for code to be cross platform
     * @param data The value to write
     * @return bool True if the value could be written
     */
    template<typename T>
    typename std::enable_if<std::is_integral_v<T>, bool>::type write(const T& data);

    /**
     * @brief Writes the string to the stream. Stores 4 bytes for the length, then the content of
     *        the string
     *
     * @param data The string to write
     * @return bool True if the value could be written
     */
    bool write(const std::string& data);

    /**
     * @brief Returns the status of the stream
     *
     */
    bool good() const;

private:
    OutputBuffer& buffer;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

inline OutputStream::OutputStream(OutputBuffer& buf)
: buffer(buf) {}

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type OutputStream::write(const T& data) {
    if (!buffer.good()) return false;

    constexpr std::size_t size = sizeof(T);
    char bytes[size];
    std::memcpy(bytes, &data, size);
    if constexpr (size > 1) {
        if (util::FileUtil::isBigEndian()) {
            for (unsigned int i = 0; i < size / 2; ++i) {
                std::swap(bytes[i], bytes[size - i - 1]);
            }
        }
    }
    return buffer.write(bytes, size);
}

inline bool OutputStream::write(const std::string& data) {
    if (!buffer.good()) return false;
    if (!write<std::uint32_t>(static_cast<std::uint32_t>(data.size()))) return false;
    return buffer.write(data.c_str(), data.size());
}

inline bool OutputStream::good() const { return buffer.good(); }

} // namespace binary
} // namespace serial
} // namespace bl

#endif
