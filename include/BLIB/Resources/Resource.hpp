#ifndef BLIB_RESOURCES_RESOURCE_HPP
#define BLIB_RESOURCES_RESOURCE_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace bl
{
namespace resource
{
/**
 * @brief Internal struct used by ResourceManager. Use Ref externally
 *
 * @tparam TResourceType the type
 * @ingroup Resources
 */
template<typename TResourceType>
struct Resource {
    TResourceType data;
    bool forceInCache;
    unsigned int refCount;

    Resource()
    : forceInCache(false)
    , refCount(1) {}

    bool readyForPurge() const { return !forceInCache && refCount == 1; }
};

/**
 * @brief Specialized Resource for sf::Font. Stores the data buffer required by the font to work
 *
 * @ingroup Resources
 */
template<>
struct Resource<sf::Font> {
    sf::Font data;
    bool forceInCache;
    unsigned int refCount;
    std::vector<char> buffer;

    Resource()
    : forceInCache(false)
    , refCount(1) {}

    bool readyForPurge() const { return !forceInCache && refCount == 1; }
};

} // namespace resource
} // namespace bl

#endif
