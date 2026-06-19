#include <BLIB/Util/StreamUtil.hpp>

namespace bl
{
namespace util
{
bool StreamUtil::skipWhitespace(std::istream& s) {
    while (s.good() && std::isspace(s.peek())) { s.get(); }
    return s.good();
}

bool StreamUtil::skipWhitespace(stream::InputStream& s) {
    while (s.isValid() && std::isspace(s.peek())) { s.get(); }
    return s.isValid();
}

bool StreamUtil::skipUntil(std::istream& s, char delim) {
    while (s.good() && s.peek() != delim) { s.get(); }
    return s.good() && s.peek() == delim;
}

bool StreamUtil::skipUntil(stream::InputStream& s, char delim) {
    while (s.isValid() && s.peek() != delim) { s.get(); }
    return s.isValid() && s.peek() == delim;
}

void StreamUtil::getline(stream::InputStream& s, std::string& output, char delim) {
    output.clear();
    while (s.isValid()) {
        const char c = s.get();
        if (c == EOF) { break; }
        if (c == delim) { break; }
        output += c;
    }
}

bool StreamUtil::isNumeric(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) || c == '-';
}

bool StreamUtil::writeRepeated(std::ostream& s, char c, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) { s << c; }
    return s.good();
}

bool StreamUtil::writeRepeated(stream::OutputStream& s, char c, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) { s << c; }
    return s.isValid();
}

} // namespace util
} // namespace bl
