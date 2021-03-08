#ifndef BLIB_RESOURCES_RESOURCE_HPP
#define BLIB_RESOURCES_RESOURCE_HPP

#include <memory>

namespace bl
{
namespace resource
{
/**
 * @brief Base level struct representing a resource that can be loaded and managed by the
 *        resource system
 *
 * @tparam TResourceType the type
 * @ingroup Resources
 */
template<typename TResourceType>
struct Resource {
    typedef std::shared_ptr<TResourceType> Ref;

    Ref data;          /// The resource itself. May not be null
    bool forceInCache; /// True to keep in cache even if no references remain

    Resource(Ref data)
    : data(data)
    , forceInCache(false) {}
};

} // namespace resource
} // namespace bl

#endif
