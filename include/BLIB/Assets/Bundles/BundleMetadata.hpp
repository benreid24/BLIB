#ifndef BLIB_ASSETS_BUNDLES_BUNDLEMETADATA_HPP
#define BLIB_ASSETS_BUNDLES_BUNDLEMETADATA_HPP

#include <BLIB/Assets/Bundles/FileMetadata.hpp>
#include <BLIB/Util/UUID.hpp>
#include <unordered_map>

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
struct BundleMetadata {
    std::unordered_map<util::UUID, std::unordered_map<std::string, FileMetadata>> assetFileManifest;
};

} // namespace bdl
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::bdl::BundleMetadata> : public SerializableObjectBase {
    SerializableField<1, as::bdl::BundleMetadata,
                      std::unordered_map<util::UUID, std::unordered_map<std::string, FileMetadata>>>
        assetFileManifest;

    SerializableObject()
    : SerializableObjectBase("BundleMetadata")
    , assetFileManifest("assetFileManifest", *this, &as::bdl::BundleMetadata::assetFileManifest,
                        SerializbleFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
