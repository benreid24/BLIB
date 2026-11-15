#include <BLIB/Serialization/Binary/Detail/OutputStreamWrapper.hpp>

namespace bl
{
namespace serial
{
namespace binary
{
namespace detail
{
OutputStreamWrapper::OutputStreamWrapper(stream::OutputStream& buf)
: buffer(buf) {}

void OutputStreamWrapper::write(const std::string& data) { write(std::string_view(data)); }

void OutputStreamWrapper::write(const char* data) { write(std::string_view(data)); }

void OutputStreamWrapper::write(std::string_view data) {
    if (!buffer.isValid()) { throw SerializationError("Cannot write to invalid stream"); }
    try {
        write<std::uint32_t>(static_cast<std::uint32_t>(data.size()));
        buffer.write(data.data(), data.size());
    } catch (const stream::StreamError& e) {
        const std::string msg = "Failed to write string to stream";
        throw SerializationError(e, msg);
    } catch (const SerializationError& e) {
        throw SerializationError(e, "Failed to write string to stream");
    }
}

} // namespace detail
} // namespace binary
} // namespace serial
} // namespace bl
