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
template<typename Payload>
struct VersionedPayloadLoader {
    virtual bool read(Payload& result, BinaryFile& input) const = 0;

    virtual bool write(const Payload& value, BinaryFile& output) const = 0;
};

template<typename Payload, typename DefaultLoader, typename... Versions>
class VersionedBinaryFile : private NonCopyable {
public:
    using Loader = VersionedPayloadLoader<Payload>;

    VersionedBinaryFile(const std::string& path);

    ~VersionedBinaryFile();

    bool write(const Payload& payload) const;

    bool read(Payload& payload) const;

private:
    static constexpr std::uint32_t Header = 3257628152;

    const std::string filename;
    const Loader* defaultLoader;
    const std::vector<const Loader*> versions;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedBinaryFile<Payload, DefaultLoader, Versions...>::VersionedBinaryFile(
    const std::string& path)
: filename(path)
, defaultLoader(new DefaultLoader())
, versions({new Versions()...}) {}

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedBinaryFile<Payload, DefaultLoader, Versions...>::~VersionedBinaryFile() {
    delete defaultLoader;
    for (const Loader* l : versions) { delete l; }
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedBinaryFile<Payload, DefaultLoader, Versions...>::write(const Payload& value) const {
    BinaryFile file(filename, BinaryFile::Write);

    const Loader* writer = versions.empty() ? defaultLoader : versions.back();
    if (!versions.empty()) {
        if (!file.write<std::uint32_t>(Header)) return false;
        if (!file.write<std::uint32_t>(versions.size() - 1)) return false;
    }
    return writer->write(value, file);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedBinaryFile<Payload, DefaultLoader, Versions...>::read(Payload& value) const {
    BinaryFile file(filename, BinaryFile::Read);

    const Loader* loader = defaultLoader;
    std::uint32_t header = 0;
    if (!file.peek<uint32_t>(header)) return false;
    if (header == Header) {
        std::uint32_t version;
        file.read<std::uint32_t>(header); // skip header
        if (!file.read<std::uint32_t>(version)) return false;
        if (version >= versions.size()) {
            BL_LOG_ERROR << "Invalid file version: file=" << filename << " version=" << version
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
