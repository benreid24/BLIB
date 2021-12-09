#ifndef BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP
#define BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP

#include <BLIB/Logging.hpp>
#include <BLIB/Serialization/Binary/InputStream.hpp>
#include <BLIB/Serialization/Binary/OutputStream.hpp>
#include <BLIB/Util/LastVariadic.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <BLIB/Util/TupleLoop.hpp>

#include <array>
#include <cstdint>
#include <tuple>
#include <utility>
#include <vector>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief Base interface for payload loaders. Version loaders should implement this interface
 *
 * @tparam Payload The type of object to read and write
 * @ingroup Binary
 */
template<typename Payload>
struct SerializerVersion {
    /**
     * @brief Destroy the Versioned Payload Loader object
     *
     */
    virtual ~SerializerVersion() = default;

    /**
     * @brief Read the object from the given file
     *
     * @param result The object to read data into
     * @param input The file to read from
     * @return True if the object was read, false on any error
     */
    virtual bool read(Payload& result, InputStream& input) const = 0;

    /**
     * @brief Write the object to the given file
     *
     * @param value The object to write
     * @param output The file to write to
     * @return True if the object was written, false on any error
     */
    virtual bool write(const Payload& value, OutputStream& output) const = 0;
};

/**
 * @brief Helper class to facilitate the versioning of binary files. Versions are represented by the
 *        provided loaders. Version numbers are derived from the template ordering so care must be
 *        taken to keep it consistent. A special header value is written to versioned files in order
 *        to detect versioned from non-versioned files
 *
 * @tparam Payload The type of object to read and write
 * @tparam DefaultLoader The SerializerVersion to use if version information is missing
 * @tparam Versions A list of SerializerVersion for all the versions of the object
 * @ingroup BinaryFiles
 */
template<typename Payload, typename DefaultLoader, typename... Versions>
class VersionedSerializer : private util::NonCopyable {
public:
    using Loader = SerializerVersion<Payload>;

    /**
     * @brief Writes the given object to the given stream
     *
     * @param output The stream to write to
     * @param payload The payload to write
     * @return True if the payload was written, false on error
     */
    static bool write(OutputStream& output, const Payload& payload);

    /**
     * @brief Reads the given object from the given stream
     *
     * @param input The stream to read from
     * @param payload The payload to read into
     * @return True if the payload was read, false on error
     */
    static bool read(InputStream& input, Payload& payload);

private:
    static constexpr std::uint32_t Header = 3257628152;

    static const Loader* getDefault();
    static const std::array<const Loader*, sizeof...(Versions)>& getLoaders();
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedSerializer<Payload, DefaultLoader, Versions...>::write(OutputStream& output,
                                                                     const Payload& value) {
    static const auto& versions        = getLoaders();
    static const Loader* defaultLoader = getDefault();
    const Loader* writer               = versions.empty() ? defaultLoader : versions.back();
    if (!versions.empty()) {
        if (!output.write<std::uint32_t>(Header)) return false;
        if (!output.write<std::uint32_t>(versions.size() - 1)) return false;
    }
    return writer->write(value, output);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedSerializer<Payload, DefaultLoader, Versions...>::read(InputStream& input,
                                                                    Payload& value) {
    static const auto& versions        = getLoaders();
    static const Loader* defaultLoader = getDefault();

    const Loader* loader = defaultLoader;
    std::uint32_t header = 0;
    if (!input.peek<uint32_t>(header)) return false;

    if (header == Header) {
        input.skip(sizeof(std::uint32_t)); // skip header

        std::uint32_t version;
        if (!input.read<std::uint32_t>(version)) return false;
        if (version >= versions.size()) {
            BL_LOG_ERROR << "Invalid file version: "
                         << " version=" << version << " max version=" << versions.size() - 1;
            return false;
        }
        loader = versions[version];
    }
    return loader->read(value, input);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
const SerializerVersion<Payload>*
VersionedSerializer<Payload, DefaultLoader, Versions...>::getDefault() {
    static const DefaultLoader loader;
    return &loader;
}

template<typename Payload, typename DefaultLoader, typename... Versions>
const std::array<const SerializerVersion<Payload>*, sizeof...(Versions)>&
VersionedSerializer<Payload, DefaultLoader, Versions...>::getLoaders() {
    static std::tuple<Versions...> loaders;
    static std::array<const Loader*, sizeof...(Versions)> loaderArray;
    static bool init = false;
    if (!init) {
        init          = true;
        const auto fn = [](std::size_t i, const Loader& l) { loaderArray[i] = &l; };
        util::TupleLoop(loaders, fn);
    }
    return loaderArray;
}

} // namespace binary
} // namespace serial
} // namespace bl

#endif
