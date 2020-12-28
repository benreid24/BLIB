#ifndef BLIB_PARSER_STREAM_HPP
#define BLIB_PARSER_STREAM_HPP

#include <ostream>
#include <sstream>

namespace bg
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
     * @brief Reads N characters ahead and returns the stream to it's prior state
     *
     * @param n How many characters to peek
     * @return std::string The result of the peek. May be smaller than n if eof is reached
     */
    std::string peekN(unsigned int n);

    /**
     * @brief Returns next character and modifies stream
     *
     * @return char Next character
     */
    char get();

    /**
     * @brief Reads N characters and returns as string
     *
     * @param n How many characters to read
     * @return std::string The read characters. May be smaller than n if eof is hit
     */
    std::string getN(unsigned int n);

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
} // namespace bg

#endif