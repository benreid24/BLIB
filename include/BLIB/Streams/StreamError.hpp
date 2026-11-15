#ifndef BLIB_STREAMS_STREAMERROR_HPP
#define BLIB_STREAMS_STREAMERROR_HPP

#include <stdexcept>
#include <string>

namespace bl
{
namespace stream
{
/**
 * @brief Error type thrown from bad stream operations
 *
 * @ingroup Streams
 */
class StreamError : public std::runtime_error {
public:
    /**
     * @brief Creates the error with the given string
     *
     * @param error The error message
     */
    StreamError(std::string&& error)
    : runtime_error(std::move(error)) {}
};

} // namespace stream
} // namespace bl

#endif
