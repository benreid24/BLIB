#ifndef BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP
#define BLIB_FILES_BINARY_VERSIONEDBINARYFILE_HPP

#include <BLIB/Files/Binary/BinaryFile.hpp>
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

    virtual bool write(Payload& value, BinaryFile& output) const = 0;
};

template<typename Payload, typename DefaultLoader, typename... Versions>
class VersionedBinaryFile : private NonCopyable {
public:
    using Loader = VersionedPayloadLoader<Payload>;

    VersionedBinaryFile(const std::string& path, BinaryFile::OpenMode mode);

    ~VersionedBinaryFile();

    bool write(const Payload& payload) const;

    bool read(Payload& payload) const;

private:
    static constexpr std::uint32_t NoVersion = 3257628152;

    BinaryFile file;
    const Loader* defaultLoader;
    const std::vector<const Loader*> versions;
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedBinaryFile<Payload, DefaultLoader, Versions...>::VersionedBinaryFile(
    const std::string& path, BinaryFile::OpenMode mode)
: file(path, mode)
, defaultLoader(new DefaultLoader())
, versions({new Versions()...}) {}

template<typename Payload, typename DefaultLoader, typename... Versions>
VersionedBinaryFile<Payload, DefaultLoader, Versions...>::~VersionedBinaryFile() {
    delete defaultLoader;
    for (const Loader* l : versions) { delete l; }
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedBinaryFile<Payload, DefaultLoader, Versions...>::write(const Payload& value) const {
    const Loader* writer = versions.empty() ? defaultLoader : versions.back();
    return writer->write(value, file);
}

template<typename Payload, typename DefaultLoader, typename... Versions>
bool VersionedBinaryFile<Payload, DefaultLoader, Versions...>::read(Payload& value) const {
    std::uint32_t version = 0;
    if (!file.peek<uint32_t>(version)) return false;
    const Loader* loader =
        (version == NoVersion || version >= versions.size()) ? defaultLoader : versions.at(version);
    if (loader != defaultLoader) file.read<uint32_t>(version); // skip
    return loader->read(value, file);
}

} // namespace bf
} // namespace bl

#endif
