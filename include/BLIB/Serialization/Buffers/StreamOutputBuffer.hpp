#ifndef BLIB_SERIALIZATION_BUFFERS_STREAMOUTPUTBUFFER_HPP
#define BLIB_SERIALIZATION_BUFFERS_STREAMOUTPUTBUFFER_HPP

#include <BLIB/Serialization/Buffers/OutputBuffer.hpp>
#include <ostream>

namespace bl
{
namespace serial
{
/**
 * @brief Output buffer that allows writing to C++ streams
 *
 * @ingroup Serialization
 *
 */
class StreamOutputBuffer : public OutputBuffer {
public:
    /**
     * @brief Construct a new Stream Output Buffer
     *
     * @param stream The stream to output to
     */
    StreamOutputBuffer(std::ostream& stream);

    /**
     * @brief Destroy the Stream Output Buffer object
     *
     */
    virtual ~StreamOutputBuffer() = default;

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

private:
    std::ostream& stream;
};

} // namespace serial
} // namespace bl

#endif
