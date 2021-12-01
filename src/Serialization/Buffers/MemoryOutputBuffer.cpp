#include <BLIB/Serialization/Buffers/MemoryOutputBuffer.hpp>

#include <cstring>

namespace bl
{
namespace serial
{
void MemoryOutputBuffer::reserve(std::size_t s) { buffer.reserve(s); }

bool MemoryOutputBuffer::write(const char* buf, std::size_t len) {
    const std::size_t pos = buffer.size();
    buffer.resize(buffer.size() + len);
    std::memcpy(&buffer[pos], buf, len);
    return true;
}

bool MemoryOutputBuffer::good() const { return true; }

const char* MemoryOutputBuffer::data() const { return buffer.data(); }

std::size_t MemoryOutputBuffer::size() const { return buffer.size(); }

} // namespace serial
} // namespace bl
