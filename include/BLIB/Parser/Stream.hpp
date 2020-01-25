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
    /**
     * @brief Construct a new Stream object
     *
     * @param data In memory string data
     */
    Stream(const std::string& data);

    /**
     * @brief Construct a new Stream object
     *
     * @param stream An existing input stream
     */
    Stream(std::istream& stream);

    /**
     * @brief Returns the next character without modifying stream
     *
     * @return char Next character
     */
    char peek() const;

    /**
     * @brief Returns next character and modifies stream
     *
     * @return char Next character
     */
    char get();

    /**
     * @brief Marks the stream as invalid
     *
     */
    void invalidate();

    /**
     * @brief Tells whether the stream is valid. Takes into account invalidation and internal
     *        stream state
     *
     * @return true Stream is valid
     * @return false Stream is invlaid
     */
    bool valid() const;

    /**
     * @brief Stream internally tracks which line it is on
     *
     * @return int The current line numer
     */
    int currentLine() const;

    /**
     * @brief Stream internall tracks which column it is on
     *
     * @return int The current column
     */
    int currentColumn() const;

private:
    std::stringstream ss;
    std::istream& stream;
    int curLine;
    int curCol;
    bool isValid;
};

} // namespace parser
} // namespace bl

#endif