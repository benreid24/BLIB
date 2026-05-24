#ifndef BLIB_STREAMS_MODE_HPP
#define BLIB_STREAMS_MODE_HPP

namespace bl
{
namespace stream
{
/**
 * @brief Represents the various modes a stream may operate in
 *
 * @ingroup Streams
 */
enum struct Mode {
    /// The stream is uninitialized or failed
    Unknown,

    /// The stream is reading from a file
    File,

    /// The stream is reading from a section of a file
    FileSection,

    /// The stream is reading from a block of memory
    Memory,

    /// The stream is wrapping an external std::istream
    Wrapper
};

} // namespace stream
} // namespace bl

#endif
