#include <BLIB/Serialization/SerializationError.hpp>

namespace bl
{
namespace serial
{
namespace
{
std::string makeNestedError(const SerializationError& nested, std::string&& error) {
    return nested.what() + std::string(" -> ") + error;
}
} // namespace

SerializationError::SerializationError(std::string&& error)
: runtime_error(std::move(error)) {}

SerializationError::SerializationError(std::string&& error, char suffix)
: runtime_error(std::move(error) + std::string(1, suffix)) {}

SerializationError::SerializationError(const SerializationError& nested, std::string&& error)
: runtime_error(makeNestedError(nested, std::move(error))) {}

SerializationError::SerializationError(const stream::StreamError& streamError)
: runtime_error(std::string("Stream error: ") + streamError.what()) {}

SerializationError::SerializationError(const stream::StreamError& streamError,
                                       const std::string& error)
: runtime_error(error + ": Stream error: " + streamError.what()) {}

SerializationError::SerializationError(const stream::StreamError& streamError, const char* error)
: runtime_error(std::string(error) + ": Stream error: " + streamError.what()) {}

} // namespace serial
} // namespace bl
