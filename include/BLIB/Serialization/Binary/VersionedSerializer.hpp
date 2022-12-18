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
#include <type_traits>
#include <utility>
#include <vector>

namespace bl
{
namespace serial
{
namespace binary
{
/// @brief Private internal namespace, do not use
namespace priv
{
template<typename T>
class ReadWriteExistChecker {
    using Pass = char;
    struct Fail {
        char x[2];
    };

    template<typename U>
    static Pass testRead(decltype(&U::read));

    template<typename U>
    static Fail testRead(...);

    template<typename U>
    static Pass testWrite(decltype(&U::write));

    template<typename U>
    static Fail testWrite(...);

public:
    static constexpr bool HasRead  = sizeof(testRead<T>(0)) == sizeof(Pass);
    static constexpr bool HasWrite = sizeof(testWrite<T>(0)) == sizeof(Pass);
};

template<typename T, typename TPayload>
struct ReadSigChecker {
    static constexpr bool Conforms =
        std::is_invocable_r_v<bool, decltype(&T::read), TPayload&, InputStream&>;
};

template<typename T, typename TPayload>
struct WriteSigChecker {
    static constexpr bool Conforms =
        std::is_invocable_r_v<bool, decltype(&T::write), const TPayload&, OutputStream&>;
};

} // namespace priv

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
    static_assert(sizeof...(Versions) >= 1, "Please provide at least one version");

    using LatestVersion                         = typename util::LastVariadic<Versions...>::Last;
    static constexpr std::uint32_t VersionCount = sizeof...(Versions);
    static constexpr std::uint32_t CurrentVersionNumber = VersionCount - 1;

    static_assert(
        priv::ReadWriteExistChecker<DefaultLoader>::HasRead,
        "Each version must have a read() method of the form: bool read(Payload&, InputStream&)");
    static_assert(
        (priv::ReadWriteExistChecker<Versions>::HasRead && ...),
        "Each version must have a read() method of the form: bool read(Payload&, InputStream&)");
    static_assert(
        (priv::ReadSigChecker<Versions, Payload>::Conforms && ...),
        "Each version must have a read() method of the form: bool read(Payload&, InputStream&)");
    static_assert(priv::WriteSigChecker<LatestVersion, Payload>::Conforms,
                  "Last version must have a write() method of the form: bool write(const Payload&, "
                  "OutputStream&)");

public:
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
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedSerializer<Payload, DefaultLoader, Versions...>::write(OutputStream& output,
                                                                     const Payload& value) {
    if (!output.write<std::uint32_t>(Header)) return false;
    if (!output.write<std::uint32_t>(CurrentVersionNumber)) return false;
    return LatestVersion::write(value, output);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedSerializer<Payload, DefaultLoader, Versions...>::read(InputStream& input,
                                                                    Payload& value) {
    constexpr bool (*loaders[])(Payload&, InputStream&) = {&Versions::read...};

    std::uint32_t header = 0;
    if (!input.peek<uint32_t>(header)) return false;

    if (header == Header) {
        input.skip(sizeof(std::uint32_t)); // skip header

        std::uint32_t version;
        if (!input.read<std::uint32_t>(version)) return false;
        if (version >= VersionCount) {
            BL_LOG_ERROR << "Invalid file version for payload" << typeid(Payload).name() << ": "
                         << " version=" << version << " max version=" << CurrentVersionNumber;
            return false;
        }
        return loaders[version](value, input);
    }
    return DefaultLoader::read(value, input);
}

} // namespace binary
} // namespace serial
} // namespace bl

#endif
