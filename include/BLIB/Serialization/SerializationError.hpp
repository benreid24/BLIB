#ifndef BLIB_SERIALIZATION_SERIALIZATIONERROR_HPP
#define BLIB_SERIALIZATION_SERIALIZATIONERROR_HPP

#include <BLIB/Streams/StreamError.hpp>
#include <stdexcept>
#include <string>

namespace bl
{
namespace serial
{
/**
 * @brief Error type for errors that occur during serialization or deserialization
 *
 * @ingroup Serialization
 */
class SerializationError : public std::runtime_error {
public:
    /**
     * @brief Creates the error with the given string
     *
     * @param error The error message
     */
    SerializationError(std::string&& error);

    /**
     * @brief Creates the error with the given string and suffic character
     *
     * @param error The error message
     * @param suffix The character to append to the error message
     */
    SerializationError(std::string&& error, char suffix);

    /**
     * @brief Creates a nested error from the given nested error and current error string
     *
     * @param nested The nested error to envelope
     * @param error The error message with the current context
     */
    SerializationError(const SerializationError& nested, std::string&& error);

    /**
     * @brief Creates from a stream error
     *
     * @param streamError The stream error to create from
     */
    SerializationError(const stream::StreamError& streamError);

    /**
     * @brief Creates from a stream error and extra error context
     *
     * @param streamError The stream error to create from
     * @param error The extra error context to append
     */
    SerializationError(const stream::StreamError& streamError, const std::string& error);

    /**
     * @brief Creates from a stream error and extra error context
     *
     * @param streamError The stream error to create from
     * @param error The extra error context to append
     */
    SerializationError(const stream::StreamError& streamError, const char* error);
};

} // namespace serial
} // namespace bl

#endif
