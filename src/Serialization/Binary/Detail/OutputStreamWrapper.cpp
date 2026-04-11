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

bool OutputStreamWrapper::write(const std::string& data) { return write(std::string_view(data)); }

bool OutputStreamWrapper::write(const char* data) { return write(std::string_view(data)); }

bool OutputStreamWrapper::write(std::string_view data) {
    if (!buffer.isValid()) return false;
    if (!write<std::uint32_t>(static_cast<std::uint32_t>(data.size()))) return false;
    return buffer.write(data.data(), data.size());
}

} // namespace detail
} // namespace binary
} // namespace serial
} // namespace bl
