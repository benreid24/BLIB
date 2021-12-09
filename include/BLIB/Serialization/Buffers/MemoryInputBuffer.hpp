#ifndef BLIB_SERIALIZATION_BUFFERS_MEMORYINPUTBUFFER_HPP
#define BLIB_SERIALIZATION_BUFFERS_MEMORYINPUTBUFFER_HPP

#include <BLIB/Serialization/Buffers/InputBuffer.hpp>
#include <SFML/Network/Packet.hpp>
#include <vector>

namespace bl
{
namespace serial
{
/**
 * @brief Input buffer that operates entirely on in-memory data
 *
 * @ingroup Serialization
 *
 */
class MemoryInputBuffer : public InputBuffer {
public:
    /**
     * @brief Construct a new Memory Input Buffer from a packet
     *
     * @param data The vector to move from
     */
    MemoryInputBuffer(const sf::Packet& packet);

    /**
     * @brief Construct a new Memory Input Buffer from a vector
     *
     * @param data The vector to copy from
     */
    MemoryInputBuffer(const std::vector<char>& data);

    /**
     * @brief Construct a new Memory Input Buffer from a raw memory buffer
     *
     * @param data Pointer to the data
     * @param len Length of the data
     */
    MemoryInputBuffer(const char* data, std::size_t len);

    /**
     * @brief Destroy the Memory Input Buffer object
     *
     */
    virtual ~MemoryInputBuffer() = default;

    /**
     * @brief Reads data from the buffer
     *
     * @param data Destination to read to
     * @param len The number of bytes to read
     * @return True on success and false on error
     */
    virtual bool read(char* data, std::size_t len) override;

    /**
     * @brief Returns the current position of the read head in the buffer
     *
     * @return std::size_t The position of the read head
     */
    virtual std::size_t tellg() const override;

    /**
     * @brief Returns the size of available input
     *
     */
    virtual std::size_t size() const override;

    /**
     * @brief Sets the position of the read head in the buffer
     *
     * @param pos The new position of the read head
     * @return True if the read head is within the bounds of the buffer, false on EOF
     */
    virtual bool seekg(std::size_t pos) override;

    /**
     * @brief Returns the next byte value to be read. May return EOF
     *
     * @return char The next byte in the buffer
     */
    virtual char peek() const override;

    /**
     * @brief Returns whether or not the buffer is in a valid state
     *
     */
    virtual bool good() const override;

private:
    const char* buffer;
    std::size_t len;
    std::size_t pos;

    MemoryInputBuffer() = delete;
};

} // namespace serial
} // namespace bl

#endif
