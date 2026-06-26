#ifndef BLIB_ASSETS_BUNDLES_MANIFEST_HPP
#define BLIB_ASSETS_BUNDLES_MANIFEST_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
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
/**
 * @brief Mapping from asset to the UUID of the bundle containing its data
 *
 * @ingroup Assets
 */
struct Manifest {
    std::unordered_map<util::UUID, util::UUID> assetToBundle;
};

} // namespace bdl
} // namespace as

namespace refl
{
template<>
struct ReflectedObject<as::bdl::Manifest> {
    inline static const auto spec = makeSpec<as::bdl::Manifest>(
        "Manifest",
        memberList(defineMember(1, "assetToBundle", &as::bdl::Manifest::assetToBundle)));
};
} // namespace refl

} // namespace bl

#endif
