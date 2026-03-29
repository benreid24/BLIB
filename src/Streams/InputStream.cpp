#include <BLIB/Streams/InputStream.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Visitor.hpp>

namespace bl
{
namespace stream
{
InputStream::InputStream()
: stream(std::monostate{})
, knownSize(0) {}

InputStream::InputStream(std::string_view path) { open(path); }

InputStream::InputStream(std::istream& stream, std::size_t size) { open(stream, size); }

InputStream::InputStream(std::span<char> data) { open(data); }

bool InputStream::isValid() const {
    return std::visit(
        util::Visitor{[](const std::monostate&) { return false; },
                      [](const std::ifstream& stream) { return stream.is_open(); },
                      [](const Buffer&) { return true; },
                      [](const std::istream* stream) { return stream != nullptr && stream->good(); }},
        stream);
}

Mode InputStream::getMode() const {
    return std::visit(util::Visitor{[](const std::monostate&) { return Mode::Unknown; },
                                    [](const std::ifstream&) { return Mode::File; },
                                    [](const Buffer&) { return Mode::Memory; },
                                    [](const std::istream*) { return Mode::Wrapper; }},
                      stream);
}

bool InputStream::open(std::string_view path) {
    auto& file = stream.emplace<std::ifstream>(std::string(path));
    if (!file.good()) { return false; }
    file.seekg(0, std::ios::end);
    knownSize = file.tellg();
    file.seekg(0, std::ios::beg);
    return true;
}

void InputStream::open(std::istream& s, std::size_t size) {
    stream.emplace<std::istream*>(&s);
    knownSize = size;
}

void InputStream::open(std::span<char> data) {
    stream.emplace<Buffer>(data);
    knownSize = data.size();
}

std::size_t InputStream::read(void* data, std::size_t len) {
    return std::visit(
        util::Visitor{[](const std::monostate&) { return static_cast<std::size_t>(0); },
                      [data, len](std::ifstream& stream) {
                          const std::size_t start = stream.tellg();
                          stream.read(static_cast<char*>(data), len);
                          return static_cast<std::size_t>(stream.tellg()) - start;
                      },
                      [data, len](Buffer& buf) {
                          const std::size_t toRead = std::min(len, buf.data.size() - buf.pos);
                          std::memcpy(data, buf.data.data() + buf.pos, toRead);
                          buf.pos += toRead;
                          return toRead;
                      },
                      [data, len](std::istream* s) -> std::size_t {
                          if (!s || !s->good()) { return static_cast<std::size_t>(0); }
                          const std::size_t start = s->tellg();
                          s->read(static_cast<char*>(data), len);
                          return static_cast<std::size_t>(s->tellg()) - start;
                      }},
        stream);
}

std::size_t InputStream::read(std::vector<char>& buf, std::size_t len) {
    buf.resize(len);
    return read(buf.data(), len);
}

std::size_t InputStream::seek(std::size_t pos) {
return std::visit(util::Visitor{[](const std::monostate&) -> std::size_t { return 0; },
                                [pos](std::ifstream& stream) -> std::size_t {
                                    stream.seekg(pos);
                                    return static_cast<std::size_t>(stream.tellg());
                                },
                                [pos](Buffer& buf) -> std::size_t {
                                    buf.pos = std::min(pos, buf.data.size());
                                    return buf.pos;
                                },
                                [pos](std::istream* s) -> std::size_t {
                                    if (s && s->good()) { s->seekg(pos); }
                                    return static_cast<std::size_t>(s->tellg());
                                }},
                  stream);
}

std::size_t InputStream::tell() const {
return std::visit(
    util::Visitor{
        [](const std::monostate&) -> std::size_t { return 0; },
        [](const std::ifstream& stream) -> std::size_t {
            return static_cast<std::size_t>(const_cast<std::ifstream&>(stream).tellg());
        },
        [](const Buffer& buf) -> std::size_t { return buf.pos; },
        [](const std::istream* s) -> std::size_t {
            return static_cast<std::size_t>(const_cast<std::istream*>(s)->tellg());
        }},
    stream);
}

} // namespace stream
} // namespace bl
