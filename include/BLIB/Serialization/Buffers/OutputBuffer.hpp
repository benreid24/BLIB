#ifndef BLIB_SERIALIZATION_BUFFERS_OUTPUTBUFFER_HPP
#define BLIB_SERIALIZATION_BUFFERS_OUTPUTBUFFER_HPP

#include <cstddef>

namespace bl
{
namespace serial
{
/**
 * @brief Interface for outputting serialized data
 *
 * @ingroup Serialization
 *
 */
class OutputBuffer {
public:
    /**
     * @brief Destroy the Output Buffer object
     *
     */
    virtual ~OutputBuffer() = default;

    /**
     * @brief Write some data to the buffer
     *
     * @param data The data to write
     * @param len The length of the data to write
     * @return True on success false on error
     */
    virtual bool write(const char* data, std::size_t len) = 0;

    /**
     * @brief Returns whether or not the buffer is in a good state
     *
     */
    virtual bool good() const = 0;
};

} // namespace serial
} // namespace bl

#endif
