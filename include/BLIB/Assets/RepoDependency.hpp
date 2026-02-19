#ifndef BLIB_ASSETS_REPODEPENDENCY_HPP
#define BLIB_ASSETS_REPODEPENDENCY_HPP

#include <BLIB/Util/UUID.hpp>
#include <string_view>

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
    std::string_view tag;
};

} // namespace as
} // namespace bl

#endif
