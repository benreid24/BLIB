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

SerializationError::SerializationError(const SerializationError& nested, std::string&& error)
: runtime_error(makeNestedError(nested, std::move(error))) {}

} // namespace serial
} // namespace bl
