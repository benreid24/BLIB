#ifndef BLIB_SERIALIZATION_BUFFERS_INPUTBUFFER_HPP
#define BLIB_SERIALIZATION_BUFFERS_INPUTBUFFER_HPP

#include <cstddef>
#include <istream>

namespace bl
{
namespace serial
{
/**
 * @brief Buffer interface for reading data to be deserialized
 *
 * @ingroup Serialization
 *
 */
class InputBuffer {
public:
    /**
     * @brief Destroy the Input Buffer object
     *
     */
    virtual ~InputBuffer() = default;

    /**
     * @brief Reads data from the buffer
     *
     * @param data Destination to read to
     * @param len The number of bytes to read
     * @return True on success and false on error
     */
    virtual bool read(char* data, std::size_t len) = 0;

    /**
     * @brief Returns the current position of the read head in the buffer
     *
     * @return std::size_t The position of the read head
     */
    virtual std::size_t tellg() const = 0;

    /**
     * @brief Returns the size of available input
     *
     */
    virtual std::size_t size() const = 0;

    /**
     * @brief Sets the position of the read head in the buffer
     *
     * @param pos The new position of the read head
     * @return True if the read head is within the bounds of the buffer, false on EOF
     */
    virtual bool seekg(std::size_t pos) = 0;

    /**
     * @brief Skips the next number of bytes
     *
     * @param bytes The number of bytes to skip
     * @return True on success false on error
     */
    bool skip(std::size_t bytes);

    /**
     * @brief Returns the next byte value to be read. May return EOF
     *
     * @return char The next byte in the buffer
     */
    virtual char peek() const = 0;

    /**
     * @brief Returns whether or not the buffer is in a valid state
     *
     */
    virtual bool good() const = 0;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

inline bool InputBuffer::skip(std::size_t o) { return seekg(tellg() + o); }

} // namespace serial
} // namespace bl

#endif
