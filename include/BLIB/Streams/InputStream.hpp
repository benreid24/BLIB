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
    InputStream(const std::string& path);

    /**
     * @brief Creates a stream for the given file path
     *
     * @param path The file to open
     */
    InputStream(std::string_view path);

    /**
     * @brief Creates a stream for the given file path
     *
     * @param path The file to open
     */
    InputStream(const char* path);

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
    InputStream(std::span<const char> data);

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
    bool open(const std::string& path);

    /**
     * @brief Opens the stream with the given file path
     *
     * @param path The path to the file to open
     * @return True if the file was successfully opened, false otherwise
     */
    bool open(std::string_view path);

    /**
     * @brief Opens the stream with the given file path
     *
     * @param path The path to the file to open
     * @return True if the file was successfully opened, false otherwise
     */
    bool open(const char* path);

    /**
     * @brief Opens a section of a file
     *
     * @param path The file to open
     * @param offset The byte offset to treat as position 0
     * @param length The number of bytes to consider the full stream length
     * @return True if the file section was successfully opened, false otherwise
     */
    bool open(const std::string& path, std::size_t offset, std::size_t length);

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
    void open(std::span<const char> data);

    /**
     * @brief Closes and invalidates the stream
     */
    void close();

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
     * @brief Peeks the next byte in the stream without advancing the position
     */
    char peek();

    /**
     * @brief Reads and returns the next byte in the stream
     */
    char get();

    /**
     * @brief Returns the total size of the stream in bytes
     */
    std::size_t getSize() const { return knownSize; }

private:
    struct Buffer {
        std::span<const char> data;
        std::size_t pos;

        Buffer(std::span<const char> data)
        : data(data)
        , pos(0) {}
    };

    struct FileSection {
        std::ifstream file;
        std::size_t offset;

        FileSection(const std::string& path, std::size_t offset)
        : file(path)
        , offset(offset) {}
    };

    std::variant<std::monostate, std::ifstream, FileSection, Buffer, std::istream*> stream;
    std::size_t knownSize;
};

} // namespace stream
} // namespace bl

#endif
