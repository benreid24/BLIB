#include <BLIB/Serialization/Buffers/StreamInputBuffer.hpp>

namespace bl
{
namespace serial
{
StreamInputBuffer::StreamInputBuffer(std::istream& s)
: stream(s) {}

bool StreamInputBuffer::read(char* data, std::size_t len) {
    stream.read(data, len);
    return stream.good();
}

std::size_t StreamInputBuffer::tellg() const { return static_cast<std::size_t>(stream.tellg()); }

std::size_t StreamInputBuffer::size() const {
    const auto pos = stream.tellg();
    stream.seekg(0, std::ios::end);
    const auto r = stream.tellg();
    stream.seekg(pos);
    return r;
}

bool StreamInputBuffer::seekg(std::size_t pos) {
    stream.seekg(static_cast<std::streampos>(pos));
    return stream.good();
}

char StreamInputBuffer::peek() const { return stream.peek(); }

bool StreamInputBuffer::good() const { return stream.good(); }

} // namespace serial
} // namespace bl
