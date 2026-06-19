#ifndef BLIB_ASSETS_REPODEPENDENCY_HPP
#define BLIB_ASSETS_REPODEPENDENCY_HPP

#include <BLIB/Serialization.hpp>
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

namespace serial
{
template<>
struct SerializableObject<as::RepoDependency> : public SerializableObjectBase {
    SerializableField<1, as::RepoDependency, util::UUID> uuid;
    SerializableField<2, as::RepoDependency, std::string> tag;

    SerializableObject()
    : SerializableObjectBase("Dependency")
    , uuid("uuid", *this, &as::RepoDependency::uuid, SerializableFieldBase::Required{})
    , tag("tag", *this, &as::RepoDependency::tag, SerializableFieldBase::Required{}) {}
};

} // namespace serial

} // namespace bl

#endif
