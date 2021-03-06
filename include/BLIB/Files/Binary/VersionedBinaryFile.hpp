#ifndef BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP
#define BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Util/LastVariadic.hpp>
#include <BLIB/Util/NonCopyable.hpp>

#include <cstdint>
#include <vector>

namespace bl
{
namespace bf
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
     * @brief Read the object from the given file
     *
     * @param result The object to read data into
     * @param input The file to read from
     * @return True if the object was read, false on any error
     */
    virtual bool read(Payload& result, BinaryFile& input) const = 0;

    /**
     * @brief Write the object to the given file
     *
     * @param value The object to write
     * @param output The file to write to
     * @return True if the object was written, false on any error
     */
    virtual bool write(const Payload& value, BinaryFile& output) const = 0;
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
class VersionedBinaryFile : private NonCopyable {
public:
    using Loader = VersionedPayloadLoader<Payload>;

    /**
     * @brief Creates instances of each loader type
     *
     */
    VersionedBinaryFile();

    /**
     * @brief Destroys each created version loader
     *
     */
    ~VersionedBinaryFile();

    /**
     * @brief Writes the given object to the given file
     *
     * @param path The file to write to
     * @param payload The payload to write
     * @return True if the payload was written, false on error
     */
    bool write(const std::string& path, const Payload& payload) const;

    /**
     * @brief Reads the given object from the given file
     *
     * @param path The file to read from
     * @param payload The payload to read into
     * @return True if the payload was read, false on error
     */
    bool read(const std::string& path, Payload& payload) const;

private:
    static constexpr std::uint32_t Header = 3257628152;

    const Loader* defaultLoader;
    const std::vector<const Loader*> versions;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedBinaryFile<Payload, DefaultLoader, Versions...>::VersionedBinaryFile()
: defaultLoader(new DefaultLoader())
, versions({new Versions()...}) {}

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedBinaryFile<Payload, DefaultLoader, Versions...>::~VersionedBinaryFile() {
    delete defaultLoader;
    for (const Loader* l : versions) { delete l; }
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedBinaryFile<Payload, DefaultLoader, Versions...>::write(const std::string& path,
                                                                     const Payload& value) const {
    BinaryFile file(path, BinaryFile::Write);

    const Loader* writer = versions.empty() ? defaultLoader : versions.back();
    if (!versions.empty()) {
        if (!file.write<std::uint32_t>(Header)) return false;
        if (!file.write<std::uint32_t>(versions.size() - 1)) return false;
    }
    return writer->write(value, file);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedBinaryFile<Payload, DefaultLoader, Versions...>::read(const std::string& path,
                                                                    Payload& value) const {
    BinaryFile file(path, BinaryFile::Read);

    const Loader* loader = defaultLoader;
    std::uint32_t header = 0;
    if (!file.peek<uint32_t>(header)) return false;
    if (header == Header) {
        std::uint32_t version;
        file.read<std::uint32_t>(header); // skip header
        if (!file.read<std::uint32_t>(version)) return false;
        if (version >= versions.size()) {
            BL_LOG_ERROR << "Invalid file version: file=" << path << " version=" << version
                         << " max version=" << versions.size() - 1;
            return false;
        }
        loader = versions.at(version);
    }
    return loader->read(value, file);
}

} // namespace bf
} // namespace bl

#endif
