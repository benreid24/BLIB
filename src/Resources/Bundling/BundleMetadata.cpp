#include <BLIB/Resources/Bundling/BundleMetadata.hpp>

namespace bl
{
namespace resource
{
namespace bundle
{
using Serializer = serial::binary::Serializer<BundleMetadata>;

void BundleMetadata::addFileInfo(const std::string& path, const BundledFileMetadata& info) {
    manifest[path] = info;
}

void BundleMetadata::applyOffset() {
    const std::size_t size = Serializer::size(*this);
    for (auto& pair : manifest) { pair.second.offset += size; }
}

BundledFileMetadata BundleMetadata::getFileInfo(const std::string& path) const {
    const auto it = manifest.find(path);
    return it != manifest.end() ? it->second : BundledFileMetadata{};
}

bool BundleMetadata::save(std::ostream& stream) const {
    serial::StreamOutputBuffer wrapper(stream);
    serial::binary::OutputStream bs(wrapper);
    return Serializer::serialize(bs, *this);
}

bool BundleMetadata::load(std::istream& stream) {
    manifest.clear();

    serial::StreamInputBuffer wrapper(stream);
    serial::binary::InputStream bs(wrapper);
    return Serializer::deserialize(bs, *this);
}

} // namespace bundle
} // namespace resource
} // namespace bl
