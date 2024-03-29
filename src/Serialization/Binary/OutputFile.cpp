#include <BLIB/Serialization/Binary/OutputFile.hpp>

namespace bl
{
namespace serial
{
namespace binary
{
OutputFile::OutputFile(const std::string& path)
: OutputStream(buffer)
, file(path)
, output(path.c_str(), std::ios::out | std::ios::binary)
, buffer(output) {}

const std::string& OutputFile::filename() const { return file; }

} // namespace binary
} // namespace serial
} // namespace bl
