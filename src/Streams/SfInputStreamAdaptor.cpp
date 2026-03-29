#include <BLIB/Streams/SfInputStreamAdaptor.hpp>

namespace bl
{
namespace stream
{
SfInputStreamAdaptor::SfInputStreamAdaptor(::bl::stream::InputStream& stream)
: stream(stream) {}

std::optional<std::size_t> SfInputStreamAdaptor::read(void* data, std::size_t size) {
    if (!stream.isValid()) { return std::nullopt; }
    return stream.read(data, size);
}

std::optional<std::size_t> SfInputStreamAdaptor::seek(std::size_t position) {
    if (!stream.isValid()) { return std::nullopt; }
    return stream.seek(position);
}

std::optional<std::size_t> SfInputStreamAdaptor::tell() {
    if (!stream.isValid()) { return std::nullopt; }
    return stream.tell();
}

std::optional<std::size_t> SfInputStreamAdaptor::getSize() {
    if (!stream.isValid()) { return std::nullopt; }
    return stream.getSize();
}

} // namespace stream
} // namespace bl
