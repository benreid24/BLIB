#ifndef BLIB_ASSETS_REPODEPENDENCY_HPP
#define BLIB_ASSETS_REPODEPENDENCY_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Util/UUID.hpp>
#include <string>

namespace bl
{
namespace as
{
/**
 * @brief Basic POD representing a stored dependency
 *
 * @ingroup Assets
 */
struct RepoDependency {
    util::UUID uuid;
    std::string tag;
};

} // namespace as

namespace refl
{
template<>
struct ReflectedObject<as::RepoDependency> {
    inline static const auto spec = makeSpec<as::RepoDependency>(
        "Dependency", memberList(defineMember(1, "uuid", &as::RepoDependency::uuid),
                                 defineMember(2, "tag", &as::RepoDependency::tag)));
};
} // namespace refl

} // namespace bl

#endif
