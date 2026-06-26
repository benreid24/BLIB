#ifndef BLIB_ASSETS_SOURCELINK_HPP
#define BLIB_ASSETS_SOURCELINK_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>
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
struct SourceLink {
    util::UUID uuid;
    std::string path;
    std::string type;

    /**
     * @brief Creates an empty source link
     */
    SourceLink()
    : uuid()
    , path("")
    , type("") {}

    /**
     * @brief Creates the source link with the given data
     *
     * @param uuid The UUID of the asset
     * @param path The path the asset was created from
     * @param type The type tag of the asset
     */
    SourceLink(util::UUID uuid, const std::string& path, std::string_view type)
    : uuid(uuid)
    , path(path)
    , type(type) {}
};

} // namespace as

namespace refl
{
template<>
struct ReflectedObject<as::SourceLink> {
    inline static const auto spec = makeSpec<as::SourceLink>(
        "SourceLink", memberList(defineMember(1, "uuid", &as::SourceLink::uuid),
                                 defineMember(2, "path", &as::SourceLink::path),
                                 defineMember(3, "type", &as::SourceLink::type)));
};
} // namespace refl

} // namespace bl

#endif
