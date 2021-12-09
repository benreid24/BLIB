#include <BLIB/Serialization/Buffers/StreamOutputBuffer.hpp>

namespace bl
{
namespace serial
{
StreamOutputBuffer::StreamOutputBuffer(std::ostream& os)
: stream(os) {}

bool StreamOutputBuffer::write(const char* buf, std::size_t len) {
    stream.write(buf, len);
    return stream.good();
}

bool StreamOutputBuffer::good() const { return stream.good(); }

} // namespace serial
} // namespace bl
