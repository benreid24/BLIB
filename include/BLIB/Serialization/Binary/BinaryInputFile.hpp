#ifndef BLIB_SERIALIZATION_BINARYINPUTFILE_HPP
#define BLIB_SERIALIZATION_BINARYINPUTFILE_HPP

#include <BLIB/Serialization/Binary/BinaryInputStream.hpp>
#include <BLIB/Serialization/Buffers/StreamInputBuffer.hpp>
#include <fstream>

namespace bl
{
namespace serial
{
/**
 * @brief Convenience wrapper around ifstream, StreamInputBuffer, and BinaryInputStream
 *
 * @ingroup Serialization
 *
 */
class BinaryInputFile : public BinaryInputStream {
public:
    /**
     * @brief Construct a new Binary Input File from a file path
     *
     * @param path The path of the file to open
     */
    BinaryInputFile(const std::string& path);

private:
    StreamInputBuffer buffer;
    std::ifstream input;
};

} // namespace serial
} // namespace bl

#endif
