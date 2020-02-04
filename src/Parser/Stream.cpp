#include <BLIB/Parser/Stream.hpp>

namespace bl
{
namespace parser
{
Stream::Stream(const std::string& data)
: ss(data)
, stream(ss)
, curLine(1)
, curCol(0)
, isValid(true) {}

Stream::Stream(std::istream& stream)
: stream(stream)
, curLine(1)
, curCol(0)
, isValid(true) {}

char Stream::peek() const { return stream.peek(); }

std::string Stream::peekN(unsigned int n) {
    std::string peek;
    int offset = 0;

    peek.reserve(n);
    while (offset < n && stream.good()) {
        const char c = stream.peek();
        peek.push_back(c);
        if (c == EOF)
            break;
        else
            stream.get();
        ++offset;
    }
    stream.seekg(-offset, std::ios_base::cur);
    return peek;
}

char Stream::get() {
    const char r = stream.get();
    if (r == '\n') {
        ++curLine;
        curCol = 0;
    }
    else
        ++curCol;
    return r;
}

std::string Stream::getN(unsigned int n) {
    std::string read;
    read.reserve(n);
    for (unsigned int i = 0; i < n; ++i) {
        if (stream.peek() == EOF) return read;
        read.push_back(get());
    }
    return read;
}

void Stream::invalidate() { isValid = false; }

bool Stream::valid() const { return stream.good() && isValid; }

int Stream::currentLine() const { return curLine; }

int Stream::currentColumn() const { return curCol; }

} // namespace parser

} // namespace bl