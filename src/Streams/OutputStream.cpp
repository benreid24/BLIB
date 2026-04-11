#include <BLIB/Streams/OutputStream.hpp>

#include <BLIB/Util/FileUtil.hpp>
#include <BLIB/Util/Visitor.hpp>

namespace bl
{
namespace stream
{
OutputStream::OutputStream()
: stream(std::monostate{}) {}

OutputStream::OutputStream(std::string_view path) { open(path); }

OutputStream::OutputStream(std::ostream& stream) { open(stream); }

OutputStream::OutputStream(std::size_t initialSize) { open(initialSize); }

Mode OutputStream::getMode() const {
    return std::visit(util::Visitor{[](const std::monostate&) { return Mode::Unknown; },
                                    [](const std::ofstream&) { return Mode::File; },
                                    [](const std::vector<char>&) { return Mode::Memory; },
                                    [](const std::ostream*) { return Mode::Wrapper; }},
                      stream);
}

bool OutputStream::isValid() const {
    return std::visit(util::Visitor{[](const std::monostate&) { return false; },
                                    [](const std::ofstream& stream) { return stream.is_open(); },
                                    [](const std::vector<char>&) { return true; },
                                    [](const std::ostream* stream) {
                                        return stream != nullptr && stream->good();
                                    }},
                      stream);
}

bool OutputStream::open(std::string_view path) {
    auto& file = stream.emplace<std::ofstream>(std::string(path));
    return file.good();
}

void OutputStream::open(std::size_t initialSize) {
    auto& buffer = stream.emplace<std::vector<char>>();
    buffer.reserve(initialSize);
}

void OutputStream::open(std::ostream& s) { stream.emplace<std::ostream*>(&s); }

void OutputStream::close() { stream.emplace<std::monostate>(); }

bool OutputStream::write(const char* data, std::size_t len) {
    return std::visit(util::Visitor{[](const std::monostate&) { return false; },
                                    [data, len](std::ofstream& stream) {
                                        stream.write(data, len);
                                        return stream.good();
                                    },
                                    [data, len](std::vector<char>& buffer) {
                                        buffer.insert(buffer.end(), data, data + len);
                                        return true;
                                    },
                                    [data, len](std::ostream* stream) {
                                        if (stream == nullptr) { return false; }
                                        stream->write(data, len);
                                        return stream->good();
                                    }},
                      stream);
}

std::span<const char> OutputStream::getBuffer() const {
    if (auto* buffer = std::get_if<std::vector<char>>(&stream)) {
        return std::span<const char>(buffer->data(), buffer->size());
    }
    return std::span<const char>();
}

} // namespace stream
} // namespace bl
