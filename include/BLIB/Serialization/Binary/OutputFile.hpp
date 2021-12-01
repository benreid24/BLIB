#ifndef BLIB_SERIALIZATION_BINARY_BINARYOUTPUTFILE_HPP
#define BLIB_SERIALIZATION_BINARY_BINARYOUTPUTFILE_HPP

#include <BLIB/Serialization/Binary/OutputStream.hpp>
#include <BLIB/Serialization/Buffers/StreamOutputBuffer.hpp>
#include <fstream>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief Helper wrapper class that encapsulates a file, a buffer, and a stream
 *
 * @ingroup Serialization
 *
 */
class OutputFile : public OutputStream {
public:
    /**
     * @brief Construct a new Output File
     *
     * @param path The file to write to
     */
    OutputFile(const std::string& path);

    /**
     * @brief The filename used to open the file
     *
     */
    const std::string& filename() const;

private:
    const std::string file;
    StreamOutputBuffer buffer;
    std::ofstream output;
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
