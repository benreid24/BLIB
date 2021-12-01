#include <BLIB/Serialization/Binary/BinaryInputFile.hpp>

namespace bl
{
namespace serial
{
BinaryInputFile::BinaryInputFile(const std::string& path)
: BinaryInputStream(buffer)
, buffer(input)
, input(path.c_str(), std::ios::in | std::ios::binary) {}

} // namespace serial
} // namespace bl
