#ifndef BLIB_PARSER_STREAM_HPP
#define BLIB_PARSER_STREAM_HPP

#include <ostream>
#include <sstream>

namespace bl
{
namespace parser
{
/**
 * @brief Wrapper class around std::ostream. Used for keeping track of current line and column
 * numbers
 * @ingroup Parser
 *
 */
class Stream {
public:
    Stream(const std::string& data);
    Stream(std::ostream& stream);

    char peek() const;
    char get();

    void invalidate();
    bool valid() const;

    int currentLine() const;
    int currentColumn() const;

private:
    std::stringstream ss;
    std::ostream& stream;
    int curLine;
    int curCol;
    bool isValid;
};

} // namespace parser
} // namespace bl

#endif