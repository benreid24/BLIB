#include <BLIB/Parser/Stream.hpp>

namespace bl
{
namespace parser
{
Stream::Stream(const std::string& data)
: ss(data)
, stream(ss)
, curLine(0)
, curCol(0)
, isValid(true) {}

Stream::Stream(std::istream& stream)
: stream(stream)
, curLine(0)
, curCol(0)
, isValid(true) {}

char Stream::peek() const { return stream.peek(); }

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

void Stream::invalidate() { isValid = false; }

bool Stream::valid() const { return stream.good() && isValid; }

int Stream::currentLine() const { return curLine; }

int Stream::currentColumn() const { return curCol; }

} // namespace parser

} // namespace bl