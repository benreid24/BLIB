#ifndef BLIB_UTIL_STREAMUTIL_HPP
#define BLIB_UTIL_STREAMUTIL_HPP

#include <BLIB/Streams/InputStream.hpp>
#include <BLIB/Streams/OutputStream.hpp>
#include <istream>
#include <ostream>

namespace bl
{
namespace util
{
/**
 * @brief Helper utility for parsing text streams
 *
 * @ingroup Util
 */
struct StreamUtil {
    /**
     * @brief Skips whitespace up until the next non-whitespace character
     *
     * @param stream The stream to skip ahead
     * @return True if the stream is in a valid state, false otherwise
     */
    static bool skipWhitespace(std::istream& stream);

    /**
     * @brief Skips whitespace up until the next non-whitespace character
     *
     * @param stream The stream to skip ahead
     * @return True if the stream is in a valid state, false otherwise
     */
    static bool skipWhitespace(stream::InputStream& stream);

    /**
     * @brief Skips all characters up until the given delimeter character
     *
     * @param stream The stream to skip ahead in
     * @param delim The character to read until. Is left in the stream
     * @return True if the stream is in a valid state, false otherwise
     */
    static bool skipUntil(std::istream& stream, char delim);

    /**
     * @brief Skips all characters up until the given delimeter character
     *
     * @param stream The stream to skip ahead in
     * @param delim The character to read until. Is left in the stream
     * @return True if the stream is in a valid state, false otherwise
     */
    static bool skipUntil(stream::InputStream& stream, char delim);

    /**
     * @brief Version of std::getline for InputStream
     *
     * @param stream The stream to read from
     * @param output The string to store the result
     * @param delim The delimiter character
     */
    static void getline(stream::InputStream& stream, std::string& output, char delim = '\n');

    /**
     * @brief Returns whether or not the given character is numeric. Allows digits and '-'
     *
     * @param c The character to test
     * @return True if the character could be the start of a number, false otherwise
     */
    static bool isNumeric(char c);

    /**
     * @brief Writes the given character into the stream a given number of times
     *
     * @param stream The stream to write into
     * @param c The character to write
     * @param count The number of times to write the character
     * @return True if the stream is in a valid state, false otherwise
     */
    static bool writeRepeated(std::ostream& stream, char c, unsigned int count);

    /**
     * @brief Writes the given character into the stream a given number of times
     *
     * @param stream The stream to write into
     * @param c The character to write
     * @param count The number of times to write the character
     * @return True if the stream is in a valid state, false otherwise
     */
    static bool writeRepeated(stream::OutputStream& stream, char c, unsigned int count);
};

} // namespace util
} // namespace bl

#endif
