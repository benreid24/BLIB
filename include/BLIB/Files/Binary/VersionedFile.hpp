#ifndef BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP
#define BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP

#include <BLIB/Files/Binary/File.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/LastVariadic.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <cstdint>
#include <vector>

namespace bl
{
namespace file
{
namespace binary
{
/**
 * @brief Base interface for payload loaders. File version loaders should implement this interface
 *
 * @tparam Payload The type of object to read and write
 * @ingroup Binary
 */
template<typename Payload>
struct VersionedPayloadLoader {
    /**
     * @brief Destroy the Versioned Payload Loader object
     *
     */
    virtual ~VersionedPayloadLoader() = default;

    /**
     * @brief Read the object from the given file
     *
     * @param result The object to read data into
     * @param input The file to read from
     * @return True if the object was read, false on any error
     */
    virtual bool read(Payload& result, File& input) const = 0;

    /**
     * @brief Write the object to the given file
     *
     * @param value The object to write
     * @param output The file to write to
     * @return True if the object was written, false on any error
     */
    virtual bool write(const Payload& value, File& output) const = 0;
};

/**
 * @brief Helper class to facilitate the versioning of binary files. Versions are represented by the
 *        provided loaders. Version numbers are derived from the template ordering so care must be
 *        taken to keep it consistent. A special header value is written to versioned files in order
 *        to detect versioned from non-versioned files
 *
 * @tparam Payload The type of object to read and write
 * @tparam DefaultLoader The VersionedPayloadLoader to use if version information is missing
 * @tparam Versions A list of VersionedPayloadLoader for all the versions of the object
 * @ingroup BinaryFiles
 */
template<typename Payload, typename DefaultLoader, typename... Versions>
class VersionedFile : private util::NonCopyable {
public:
    using Loader = VersionedPayloadLoader<Payload>;

    /**
     * @brief Creates instances of each loader type
     *
     */
    VersionedFile();

    /**
     * @brief Destroys each created version loader
     *
     */
    ~VersionedFile();

    /**
     * @brief Writes the given object to the given file
     *
     * @param output The file to write to
     * @param payload The payload to write
     * @return True if the payload was written, false on error
     */
    bool write(File& output, const Payload& payload) const;

    /**
     * @brief Reads the given object from the given file
     *
     * @param input The file to read from
     * @param payload The payload to read into
     * @return True if the payload was read, false on error
     */
    bool read(File& input, Payload& payload) const;

private:
    static constexpr std::uint32_t Header = 3257628152;

    const Loader* defaultLoader;
    const std::vector<const Loader*> versions;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedFile<Payload, DefaultLoader, Versions...>::VersionedFile()
: defaultLoader(new DefaultLoader())
, versions({new Versions()...}) {}

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedFile<Payload, DefaultLoader, Versions...>::~VersionedFile() {
    delete defaultLoader;
    for (const Loader* l : versions) { delete l; }
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedFile<Payload, DefaultLoader, Versions...>::write(File& output,
                                                               const Payload& value) const {
    const Loader* writer = versions.empty() ? defaultLoader : versions.back();
    if (!versions.empty()) {
        if (!output.write<std::uint32_t>(Header)) return false;
        if (!output.write<std::uint32_t>(versions.size() - 1)) return false;
    }
    return writer->write(value, output);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedFile<Payload, DefaultLoader, Versions...>::read(File& input, Payload& value) const {
    const Loader* loader = defaultLoader;
    std::uint32_t header = 0;
    if (!input.peek<uint32_t>(header)) return false;
    if (header == Header) {
        std::uint32_t version;
        input.read<std::uint32_t>(header); // skip header
        if (!input.read<std::uint32_t>(version)) return false;
        if (version >= versions.size()) {
            BL_LOG_ERROR << "Invalid file version: file=" << input.filename()
                         << " version=" << version << " max version=" << versions.size() - 1;
            return false;
        }
        loader = versions.at(version);
    }
    return loader->read(value, input);
}

} // namespace binary
} // namespace file
} // namespace bl

#endif
