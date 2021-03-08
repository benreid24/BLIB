#ifndef BLIB_RESOURCES_RESOURCELOADER_HPP
#define BLIB_RESOURCES_RESOURCELOADER_HPP

#include <BLIB/Resources/Resource.hpp>

#include <string>

namespace bl
{
namespace resource
{
/**
 * @brief Templatized resource loader. Must be implemented for any resource type that is
 *        intended to be managed
 *
 * @see TextureResourceLoader
 * @see FontResourceLoader
 * @ingroup Resources
 */
template<typename TResourceType>
struct Loader {
    virtual ~Loader() = default;

    /**
     * @brief Load the resource as identified by the uri
     *
     * @param uri The resource identifier
     * @return Resource<TResourceType>::Ref The loaded resource
     */
    virtual typename Resource<TResourceType>::Ref load(const std::string& uri) = 0;
};

} // namespace resource
} // namespace bl

#endif
