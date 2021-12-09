#ifndef BLIB_SERIALIZATION_BINARY_BINARYINPUTFILE_HPP
#define BLIB_SERIALIZATION_BINARY_BINARYINPUTFILE_HPP

#include <BLIB/Serialization/Binary/InputStream.hpp>
#include <BLIB/Serialization/Buffers/StreamInputBuffer.hpp>
#include <fstream>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief Convenience wrapper around ifstream, StreamInputBuffer, and BinaryInputStream
 *
 * @ingroup Binary
 *
 */
class InputFile : public InputStream {
public:
    /**
     * @brief Construct a new Binary Input File from a file path
     *
     * @param path The path of the file to open
     */
    InputFile(const std::string& path);

    /**
     * @brief Returns the filename used to open the file
     *
     */
    const std::string& filename() const;

private:
    const std::string file;
    StreamInputBuffer buffer;
    std::ifstream input;
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
