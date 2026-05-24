#ifndef BLIB_ASSETS_BUNDLES_BUNDLADATA_HPP
#define BLIB_ASSETS_BUNDLES_BUNDLADATA_HPP

#include <BLIB/Assets/Bundles/FileMetadata.hpp>
#include <BLIB/Util/UUID.hpp>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace as
{
namespace bdl
{
/**
 * @brief Metadata for a single bundle file
 *
 * @ingroup Assets
 */
struct BundleData {
    std::uint32_t headerSize;
    std::unordered_map<util::UUID, std::unordered_map<std::string, FileMetadata>> assetFileManifest;
    std::vector<char> data;
};

} // namespace bdl
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::bdl::BundleData> : public SerializableObjectBase {
    SerializableField<1, as::bdl::BundleData, std::uint32_t> headerSize;
    SerializableField<
        2, as::bdl::BundleData,
        std::unordered_map<util::UUID, std::unordered_map<std::string, as::bdl::FileMetadata>>>
        assetFileManifest;
    SerializableField<3, as::bdl::BundleData, std::vector<char>> data;

    SerializableObject()
    : SerializableObjectBase("BundleData")
    , headerSize("headerSize", *this, &as::bdl::BundleData::headerSize,
                 SerializableFieldBase::Required{})
    , assetFileManifest("assetFileManifest", *this, &as::bdl::BundleData::assetFileManifest,
                        SerializableFieldBase::Required{})
    , data("data", *this, &as::bdl::BundleData::data, SerializableFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
