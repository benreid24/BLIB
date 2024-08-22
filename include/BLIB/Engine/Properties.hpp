#ifndef BLIB_ENGINE_PROPERTIES_HPP
#define BLIB_ENGINE_PROPERTIES_HPP

#include <BLIB/Engine/Property.hpp>

namespace bl
{
namespace engine
{
/**
 * @brief Helper for managing application properties
 *
 * @ingroup Engine
 */
class Properties : private util::NonCopyable {
public:
    /**
     * @brief Syncs all current instances of Property to Configuration
     */
    static void syncToConfig();

    /**
     * @brief Syncs all current instances of Property from Configuration
     */
    static void syncFromConfig();

private:
    static void addProperty(priv::PropertyBase* property);
    static void removeProperty(priv::PropertyBase* property);

    friend class priv::PropertyBase;
};

} // namespace engine
} // namespace bl

#endif
