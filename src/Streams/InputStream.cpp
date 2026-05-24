#include <BLIB/Streams/InputStream.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <cstring>

namespace bl
{
namespace stream
{
InputStream::InputStream()
: stream(std::monostate{})
, knownSize(0) {}

InputStream::InputStream(const std::string& path) { open(path); }

InputStream::InputStream(std::string_view path) { open(std::string(path)); }

InputStream::InputStream(const char* path) { open(path); }

InputStream::InputStream(std::istream& stream, std::size_t size) { open(stream, size); }

InputStream::InputStream(std::span<const char> data) { open(data); }

bool InputStream::isValid() const {
    return std::visit(
        util::Visitor{
            [](const std::monostate&) { return false; },
            [](const std::ifstream& stream) { return stream.is_open(); },
            [](const FileSection& section) { return section.file.is_open(); },
            [](const Buffer&) { return true; },
            [](const std::istream* stream) { return stream != nullptr && stream->good(); }},
        stream);
}

Mode InputStream::getMode() const {
    return std::visit(util::Visitor{[](const std::monostate&) { return Mode::Unknown; },
                                    [](const std::ifstream&) { return Mode::File; },
                                    [](const FileSection&) { return Mode::FileSection; },
                                    [](const Buffer&) { return Mode::Memory; },
                                    [](const std::istream*) { return Mode::Wrapper; }},
                      stream);
}

bool InputStream::open(std::string_view path) { return open(std::string(path)); }

bool InputStream::open(const char* path) { return open(std::string(path)); }

bool InputStream::open(const std::string& path) {
    auto& file = stream.emplace<std::ifstream>(path);
    if (!file.good()) { return false; }
    file.seekg(0, std::ios::end);
    knownSize = file.tellg();
    file.seekg(0, std::ios::beg);
    return true;
}

bool InputStream::open(const std::string& path, std::size_t offset, std::size_t length) {
    auto& section = stream.emplace<FileSection>(path, offset);
    if (!section.file.good()) { return false; }
    section.file.seekg(offset + length);
    if (!section.file.good()) { return false; }
    section.file.seekg(offset);
    knownSize = length;
    return true;
}

void InputStream::open(std::istream& s, std::size_t size) {
    stream.emplace<std::istream*>(&s);
    knownSize = size;
}

void InputStream::open(std::span<const char> data) {
    stream.emplace<Buffer>(data);
    knownSize = data.size();
}

void InputStream::close() {
    stream.emplace<std::monostate>();
    knownSize = 0;
}

std::size_t InputStream::read(void* data, std::size_t len) {
    return std::visit(
        util::Visitor{[](const std::monostate&) { return static_cast<std::size_t>(0); },
                      [data, len](std::ifstream& stream) -> std::size_t {
                          stream.read(static_cast<char*>(data), len);
                          return stream.gcount();
                      },
                      [this, data, len](FileSection& section) {
                          const std::size_t globalPos = section.file.tellg();
                          const std::size_t localPos  = globalPos - section.offset;
                          const std::size_t toRead    = std::min(len, knownSize - localPos);
                          section.file.read(static_cast<char*>(data), toRead);
                          return toRead;
                      },
                      [data, len](Buffer& buf) -> std::size_t {
                          const std::size_t toRead = std::min(len, buf.data.size() - buf.pos);
                          std::memcpy(data, buf.data.data() + buf.pos, toRead);
                          buf.pos += toRead;
                          return toRead;
                      },
                      [data, len](std::istream* s) -> std::size_t {
                          if (!s || !s->good()) { return static_cast<std::size_t>(0); }
                          s->read(static_cast<char*>(data), len);
                          return s->gcount();
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
                                    [pos](FileSection& section) -> std::size_t {
                                        section.file.seekg(section.offset + pos);
                                        return static_cast<std::size_t>(section.file.tellg()) -
                                               section.offset;
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
            [](const FileSection& section) -> std::size_t {
                return static_cast<std::size_t>(const_cast<FileSection&>(section).file.tellg()) -
                       section.offset;
            },
            [](const Buffer& buf) -> std::size_t { return buf.pos; },
            [](const std::istream* s) -> std::size_t {
                return static_cast<std::size_t>(const_cast<std::istream*>(s)->tellg());
            }},
        stream);
}

char InputStream::peek() {
    return std::visit(util::Visitor{[](const std::monostate&) -> char { return EOF; },
                                    [](std::ifstream& stream) -> char { return stream.peek(); },
                                    [this](FileSection& section) -> char {
                                        const std::size_t pos = tell();
                                        return pos < knownSize ? section.file.peek() : EOF;
                                    },
                                    [](Buffer& buf) -> char {
                                        if (buf.pos < buf.data.size()) { return buf.data[buf.pos]; }
                                        else { return EOF; }
                                    },
                                    [](std::istream* s) -> char {
                                        if (s) { return s->peek(); }
                                        else { return EOF; }
                                    }},
                      stream);
}

char InputStream::get() {
    return std::visit(util::Visitor{[](const std::monostate&) -> char { return EOF; },
                                    [](std::ifstream& stream) -> char { return stream.get(); },
                                    [this](FileSection& section) -> char {
                                        const std::size_t pos = tell();
                                        return pos < knownSize ? section.file.get() : EOF;
                                    },
                                    [](Buffer& buf) -> char {
                                        if (buf.pos < buf.data.size()) {
                                            return buf.data[buf.pos++];
                                        }
                                        else { return EOF; }
                                    },
                                    [](std::istream* s) -> char {
                                        if (s) { return s->get(); }
                                        else { return EOF; }
                                    }},
                      stream);
}

} // namespace stream
} // namespace bl
