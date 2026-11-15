#include <BLIB/Streams/StreamUtil.hpp>

#include <BLIB/Streams/StreamError.hpp>

namespace bl
{
namespace stream
{
void StreamUtil::skipWhitespace(std::istream& s) {
    while (s.good() && std::isspace(s.peek())) { s.get(); }
    if (!s.good()) { throw StreamError("Failed to skip whitespace, stream is in a bad state"); }
}

void StreamUtil::skipWhitespace(stream::InputStream& s) {
    while (s.isValid() && std::isspace(s.peek())) { s.get(); }
    if (!s.isValid()) { throw StreamError("Failed to skip whitespace, stream is in a bad state"); }
}

void StreamUtil::skipUntil(std::istream& s, char delim) {
    while (s.good() && s.peek() != delim) { s.get(); }
    if (!s.good() || s.peek() != delim) {
        throw StreamError("Failed to skip until delimiter, stream is in a bad state");
    }
}

void StreamUtil::skipUntil(stream::InputStream& s, char delim) {
    while (s.isValid() && s.peek() != delim) { s.get(); }
    if (!s.isValid() || s.peek() != delim) {
        throw StreamError("Failed to skip until delimiter, stream is in a bad state");
    }
}

void StreamUtil::getline(stream::InputStream& s, std::string& output, char delim) {
    output.clear();
    while (s.isValid()) {
        const char c = s.get();
        if (c == EOF) { throw StreamError("Failed to read line, unexpected EOF"); }
        if (c == delim) { return; }
        output += c;
    }
    throw StreamError("Failed to read line, stream is in a bad state");
}

bool StreamUtil::isNumeric(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) || c == '-';
}

void StreamUtil::writeRepeated(std::ostream& s, char c, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) { s << c; }
    if (!s.good()) {
        throw StreamError("Failed to write repeated characters, stream is in a bad state");
    }
}

void StreamUtil::writeRepeated(stream::OutputStream& s, char c, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) { s << c; }
    if (!s.isValid()) {
        throw StreamError("Failed to write repeated characters, stream is in a bad state");
    }
}

} // namespace stream
} // namespace bl
