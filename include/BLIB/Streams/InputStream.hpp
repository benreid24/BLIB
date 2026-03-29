#ifndef BLIB_STREAMS_INPUTSTREAM_HPP
#define BLIB_STREAMS_INPUTSTREAM_HPP

#include <BLIB/Streams/Mode.hpp>
#include <fstream>
#include <span>
#include <string_view>
#include <variant>
#include <vector>

namespace bl
{
/// Contains custom streams
namespace stream
{
/**
 * @brief Universal input stream which can operate on files, buffers, and other streams
 *
 * @ingroup Streams
 */
class InputStream {
public:
    /**
     * @brief Creates an uninitialized stream
     */
    InputStream();

    /**
     * @brief Creates a stream for the given file path
     *
     * @param path The file to open
     */
    InputStream(std::string_view path);

    /**
     * @brief Creates a stream wrapping an existing stream
     *
     * @param stream The stream to wrap
     * @param knownSize The size of the stream in bytes
     */
    InputStream(std::istream& stream, std::size_t knownSize);

    /**
     * @brief Creates a stream from a buffer
     *
     * @param data The buffer to wrap
     */
    InputStream(std::span<char> data);

    /**
     * @brief Destroys the stream
     */
    ~InputStream() = default;

    /**
     * @brief Returns the mode of the stream
     */
    Mode getMode() const;

    /**
     * @brief Returns whether the stream is in a valid state for reading
     */
    bool isValid() const;

    /**
     * @brief Returns whether the stream is in a valid state for reading
     */
    operator bool() const { return isValid(); }

    /**
     * @brief Opens the stream with the given file path
     *
     * @param path The path to the file to open
     * @return True if the file was successfully opened, false otherwise
     */
    bool open(std::string_view path);

    /**
     * @brief Opens the stream as a wrapper over the existing stream
     *
     * @param stream The stream to wrap
     * @param knownSize The size of the stream in bytes
     */
    void open(std::istream& stream, std::size_t knownSize);

    /**
     * @brief Opens the stream as a wrapper over the given buffer
     *
     * @param data The buffer to wrap
     */
    void open(std::span<char> data);

    /**
     * @brief Reads data from the stream into the given buffer
     *
     * @param data Pointer to the buffer to read into
     * @param len The number of bytes to read
     * @return The number of bytes read
     */
    std::size_t read(void* data, std::size_t len);

    /**
     * @brief Reads the data from the stream into the given buffer
     *
     * @param buffer The buffer to read into
     * @param len The number of bytes to read
     * @return The number of bytes read
     */
    std::size_t read(std::vector<char>& buffer, std::size_t len);

    /**
     * @brief Seeks to the given position in bytes
     *
     * @param pos The byte position to seek to
     * @return The position actually seeked to
     */
    std::size_t seek(std::size_t pos);

    /**
     * @brief Returns the current byte position in the stream
     */
    std::size_t tell() const;

    /**
     * @brief Returns the total size of the stream in bytes
     */
    std::size_t getSize() const { return knownSize; }

private:
    struct Buffer {
        std::span<char> data;
        std::size_t pos;

        Buffer(std::span<char> data)
        : data(data)
        , pos(0) {}
    };

    std::variant<std::monostate, std::ifstream, Buffer, std::istream*> stream;
    std::size_t knownSize;
};

} // namespace stream
} // namespace bl

#endif
