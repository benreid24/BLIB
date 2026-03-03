#include <BLIB/Serialization/Binary/OutputStream.hpp>

namespace bl
{
namespace serial
{
namespace binary
{
OutputStream::OutputStream(OutputBuffer& buf)
: buffer(buf) {}

bool OutputStream::write(const std::string& data) { return write(std::string_view(data)); }

bool OutputStream::write(std::string_view data) {
    if (!buffer.good()) return false;
    if (!write<std::uint32_t>(static_cast<std::uint32_t>(data.size()))) return false;
    return buffer.write(data.data(), data.size());
}

} // namespace binary
} // namespace serial
} // namespace bl
