#ifndef BLIB_SERIALIZATION_BUFFERS_MEMORYOUTPUTBUFFER_HPP
#define BLIB_SERIALIZATION_BUFFERS_MEMORYOUTPUTBUFFER_HPP

#include <BLIB/Serialization/Buffers/OutputBuffer.hpp>
#include <vector>

namespace bl
{
namespace serial
{
/**
 * @brief Buffer that allows serializing data to an in-memory buffer
 *
 * @ingroup Serialization
 *
 */
class MemoryOutputBuffer : public OutputBuffer {
public:
    /**
     * @brief Construct a new Memory Output Buffer
     *
     */
    MemoryOutputBuffer() = default;

    /**
     * @brief Destroy the Memory Output Buffer object
     *
     */
    virtual ~MemoryOutputBuffer() = default;

    /**
     * @brief Reserves capacity for the serialized output
     *
     * @param capacity The number of bytes to ensure are available
     */
    void reserve(std::size_t capacity);

    /**
     * @brief Write some data to the buffer
     *
     * @param data The data to write
     * @param len The length of the data to write
     * @return True on success false on error
     */
    virtual bool write(const char* data, std::size_t len) override;

    /**
     * @brief Returns whether or not the buffer is in a good state
     *
     */
    virtual bool good() const override;

    /**
     * @brief Returns a pointer to the contained data
     *
     */
    const char* data() const;

    /**
     * @brief Returns the size of the buffer
     *
     */
    std::size_t size() const;

private:
    std::vector<char> buffer;
};

} // namespace serial
} // namespace bl

#endif
