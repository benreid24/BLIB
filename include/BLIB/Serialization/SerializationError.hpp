#ifndef BLIB_SERIALIZATION_SERIALIZATIONERROR_HPP
#define BLIB_SERIALIZATION_SERIALIZATIONERROR_HPP

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
     * @brief Creates a nested error from the given nested error and current error string
     *
     * @param nested The nested error to envelope
     * @param error The error message with the current context
     */
    SerializationError(const SerializationError& nested, std::string&& error);
};

} // namespace serial
} // namespace bl

#endif
