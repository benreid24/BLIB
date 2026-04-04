#ifndef BLIB_STREAMS_OUTPUTSTREAM_HPP
#define BLIB_STREAMS_OUTPUTSTREAM_HPP

#include <BLIB/Streams/Mode.hpp>
#include <fstream>
#include <string_view>
#include <variant>
#include <vector>

namespace bl
{
namespace stream
{
/**
 * @brief Universal output stream which can operate on files, buffers, and other streams
 *
 * @ingroup Streams
 */
class OutputStream {
public:
    /**
     * @brief Creates an uninitialized stream
     */
    OutputStream();

    /**
     * @brief Creates a stream for the given file path
     *
     * @param path The file to open
     */
    OutputStream(std::string_view path);

    /**
     * @brief Creates a stream wrapping an existing stream
     *
     * @param stream The stream to wrap
     */
    OutputStream(std::ostream& stream);

    /**
     * @brief Creates a stream with an internal buffer with the given initial size
     *
     * @param initialSize The initial size to reserve for the internal buffer
     */
    OutputStream(std::size_t initialSize);

    /**
     * @brief Destroys the stream
     */
    ~OutputStream() = default;

    /**
     * @brief Returns the mode of the stream
     */
    Mode getMode() const;

    /**
     * @brief Returns whether or not the stream is in a good state
     */
    bool isValid() const;

    /**
     * @brief Returns whether or not the stream is in a good state
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
     * @brief Creates a stream with an internal buffer with the given initial size
     *
     * @param initialSize The initial size to reserve for the internal buffer
     */
    void open(std::size_t initialSize);

    /**
     * @brief Opens the stream as a wrapper over the existing stream
     *
     * @param stream The stream to wrap
     */
    void open(std::ostream& stream);

    /**
     * @brief Closes and invalidates the stream
     */
    void close();

    /**
     * @brief Writes data into the underlying stream
     *
     * @param data The data to write
     * @param len The number of bytes to write
     * @return True if the data was successfully written, false otherwise
     */
    bool write(const char* data, std::size_t len);

    /**
     * @brief Returns the underlying buffer. Only valid if the stream is in Memory mode
     */
    const std::vector<char>* getBuffer() const;

private:
    std::variant<std::monostate, std::ofstream, std::ostream*, std::vector<char>> stream;
};

} // namespace stream
} // namespace bl

#endif
