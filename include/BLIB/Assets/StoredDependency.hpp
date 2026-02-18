#ifndef BLIB_ASSETS_STOREDDEPENDENCY_HPP
#define BLIB_ASSETS_STOREDDEPENDENCY_HPP

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
struct StoredDependency {
    util::UUID uuid;
    std::string_view tag;
};
} // namespace as
} // namespace bl

#endif
