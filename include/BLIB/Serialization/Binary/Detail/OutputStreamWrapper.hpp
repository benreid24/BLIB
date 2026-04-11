#ifndef BLIB_SERIALIZATION_BINARY_DETAIL_OUTPUTSTREAMWRAPPER_HPP
#define BLIB_SERIALIZATION_BINARY_DETAIL_OUTPUTSTREAMWRAPPER_HPP

#include <BLIB/Streams/OutputStream.hpp>
#include <BLIB/Util/FileUtil.hpp>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

namespace bl
{
namespace serial
{
namespace binary
{
namespace detail
{
/**
 * @brief A writable stream for binary serialization
 *
 * @ingroup Binary
 *
 */
class OutputStreamWrapper {
public:
    /**
     * @brief Construct a new stream wrapper
     *
     * @param stream The underlying stream to write to
     */
    OutputStreamWrapper(stream::OutputStream& stream);

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
     * @brief Writes the string to the stream. Stores 4 bytes for the length, then the content of
     *        the string
     *
     * @param data The string to write
     * @return bool True if the value could be written
     */
    bool write(std::string_view data);

    /**
     * @brief Writes the string to the stream. Stores 4 bytes for the length, then the content of
     *        the string
     *
     * @param data The string to write
     * @return bool True if the value could be written
     */
    bool write(const char* data);

    /**
     * @brief Returns the status of the stream
     *
     */
    bool good() const;

private:
    stream::OutputStream& buffer;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
typename std::enable_if<std::is_integral_v<T>, bool>::type OutputStreamWrapper::write(
    const T& data) {
    if (!buffer.isValid()) return false;

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

inline bool OutputStreamWrapper::good() const { return buffer.isValid(); }

} // namespace detail
} // namespace binary
} // namespace serial
} // namespace bl

#endif
