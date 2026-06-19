#ifndef BLIB_STREAMS_SFINPUTSTREAMADAPTOR_HPP
#define BLIB_STREAMS_SFINPUTSTREAMADAPTOR_HPP

#include <BLIB/Streams/InputStream.hpp>
#include <SFML/System/InputStream.hpp>

namespace bl
{
namespace stream
{
/**
 * @brief Adaptor class to wrap an InputStream for use with SFML
 *
 * @ingroup Streams
 */
class SfInputStreamAdaptor : public sf::InputStream {
public:
    /**
     * @brief Creates the stream adaptor
     *
     * @param stream The stream to wrap. Must remain valid for the lifetime of this adaptor
     */
    SfInputStreamAdaptor(::bl::stream::InputStream& stream);

    /**
     * @brief Reads data from the underlying stream
     *
     * @param data Pointer of the buffer to read into
     * @param size The number of bytes to read
     * @return The number of bytes read, or `std::nullopt` on error
     */
    virtual std::optional<std::size_t> read(void* data, std::size_t size) override;

    /**
     * @brief Seeks to the specified position in the stream
     * @param position The zero-based byte offset to seek to
     *
     * @return The new position in the stream if successful, or std::nullopt on error
     */
    virtual std::optional<std::size_t> seek(std::size_t position) override;

    /**
     * @brief Returns the current position in the stream
     *
     * @return The current position in the stream if successful, or std::nullopt on error
     */
    virtual std::optional<std::size_t> tell() override;

    /**
     * @brief Returns the total size of the stream
     *
     * @return The total size of the stream if successful, or std::nullopt on error
     */
    virtual std::optional<std::size_t> getSize() override;

private:
    ::bl::stream::InputStream& stream;
};

} // namespace stream
} // namespace bl

#endif
