#include <BLIB/Serialization/Binary/InputFile.hpp>

namespace bl
{
namespace serial
{
namespace binary
{
InputFile::InputFile(const std::string& path)
: InputStream(buffer)
, file(path)
, input(path.c_str(), std::ios::in | std::ios::binary)
, buffer(input) {}

const std::string& InputFile::filename() const { return file; }

} // namespace binary
} // namespace serial
} // namespace bl
