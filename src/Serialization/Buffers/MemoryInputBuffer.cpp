#include <BLIB/Serialization/Buffers/MemoryInputBuffer.hpp>

#include <cstring>

namespace bl
{
namespace serial
{
MemoryInputBuffer::MemoryInputBuffer(const sf::Packet& packet)
: buffer(static_cast<const char*>(packet.getData()))
, len(packet.getDataSize())
, pos(0) {}

MemoryInputBuffer::MemoryInputBuffer(const std::vector<char>& v)
: buffer(v.data())
, len(v.size())
, pos(0) {}

MemoryInputBuffer::MemoryInputBuffer(const char* data, std::size_t l)
: buffer(data)
, len(l)
, pos(0) {}

bool MemoryInputBuffer::read(char* data, std::size_t s) {
    const std::size_t r = std::min(len - pos, s);
    std::memcpy(data, &buffer[pos], r);
    pos += r;
    return r == s;
}

std::size_t MemoryInputBuffer::tellg() const { return pos; }

std::size_t MemoryInputBuffer::size() const { return len; }

bool MemoryInputBuffer::seekg(std::size_t p) {
    pos = std::min(p, len);
    return pos <= len;
}

char MemoryInputBuffer::peek() const { return pos < len ? buffer[pos] : EOF; }

bool MemoryInputBuffer::good() const { return pos < len; }

} // namespace serial
} // namespace bl
