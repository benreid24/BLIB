#ifndef BLIB_ASSETS_STATICASSET_HPP
#define BLIB_ASSETS_STATICASSET_HPP

#include <BLIB/Serialization.hpp>
#include <BLIB/Util/UUID.hpp>

namespace bl
{
namespace as
{
/**
 * @brief Represents a fixed asset referenced via file path from the game source directory
 *
 * @ingroup Assets
 */
struct StaticAsset {
    util::UUID uuid;
    std::string path;
    std::string type;

    /**
     * @brief Creates an empty static asset
     */
    StaticAsset()
    : uuid()
    , path("")
    , type("") {}

    /**
     * @brief Creates the static asset with the given data
     *
     * @param uuid The UUID of the asset
     * @param path The path the asset was created from
     * @param type The type tag of the asset
     */
    StaticAsset(util::UUID uuid, const std::string& path, std::string_view type)
    : uuid(uuid)
    , path(path)
    , type(type) {}
};

} // namespace as

namespace serial
{
template<>
struct SerializableObject<as::StaticAsset> : public SerializableObjectBase {
    SerializableField<1, as::StaticAsset, util::UUID> uuid;
    SerializableField<2, as::StaticAsset, std::string> path;
    SerializableField<3, as::StaticAsset, std::string> type;

    SerializableObject()
    : SerializableObjectBase("StaticAsset")
    , uuid("uuid", *this, &as::StaticAsset::uuid, SerializableFieldBase::Required{})
    , path("path", *this, &as::StaticAsset::path, SerializableFieldBase::Required{})
    , type("type", *this, &as::StaticAsset::type, SerializableFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
