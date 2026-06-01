#include <BLIB/Assets/Bundles/BundleData.hpp>

#include <BLIB/Assets/Bundles/RuntimePaths.hpp>

namespace bl
{
namespace as
{
namespace bdl
{
BundleData::BundleData()
: headerSize(0)
, uuid(util::UUID::generate()) {}

bool BundleData::flush(const std::string& base) {
    stream::OutputStream output(RuntimePaths::getBundlePath(base, uuid));
    if (!output.isValid()) { return false; }

    headerSize = serial::binary::Serializer<BundleData>::size(*this) - data.size();
    return serial::binary::Serializer<BundleData>::serialize(output, *this);
}

void BundleData::reset() {
    uuid = util::UUID::generate();
    data.clear();
    assetFileManifest.clear();
    headerSize = 0;
}

} // namespace bdl
} // namespace as
} // namespace bl
