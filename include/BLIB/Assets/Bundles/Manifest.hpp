#ifndef BLIB_ASSETS_BUNDLES_MANIFEST_HPP
#define BLIB_ASSETS_BUNDLES_MANIFEST_HPP

#include <BLIB/Assets/Bundles/BundleMetadata.hpp>
#include <BLIB/Serialization.hpp>
#include <BLIB/Util/UUID.hpp>
#include <string>
#include <unordered_map>

namespace bl
{
namespace as
{
/// Collection of classes for asset bundling
namespace bdl
{
struct Manifest {
    std::unordered_map<util::UUID, util::UUID> assetToBundle;
};

} // namespace bdl
} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::bdl::Manifest> : public SerializableObjectBase {
    SerializableField<1, as::bdl::Manifest, std::unordered_map<util::UUID, util::UUID>>
        assetToBundle;

    SerializableObject()
    : SerializableObjectBase("Manifest")
    , assetToBundle("assetToBundle", *this, &as::bdl::Manifest::assetToBundle,
                    SerializableFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
