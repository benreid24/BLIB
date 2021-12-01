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
, buffer(input)
, input(path.c_str(), std::ios::in | std::ios::binary) {}

const std::string& InputFile::filename() const { return file; }

} // namespace binary
} // namespace serial
} // namespace bl
