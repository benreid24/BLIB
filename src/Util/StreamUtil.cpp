#include <BLIB/Util/StreamUtil.hpp>

namespace bl
{
namespace util
{
bool StreamUtil::skipWhitespace(std::istream& s) {
    while (s.good() && std::isspace(s.peek())) { s.get(); }
    return s.good();
}

bool StreamUtil::skipUntil(std::istream& s, char delim) {
    while (s.good() && s.peek() != delim) { s.get(); }
    return s.good() && s.peek() == delim;
}

bool StreamUtil::isNumeric(char c) {
    return std::isdigit(static_cast<unsigned char>(c)) || c == '-';
}

bool StreamUtil::writeRepeated(std::ostream& s, char c, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) { s << c; }
    return s.good();
}

} // namespace util
} // namespace bl
