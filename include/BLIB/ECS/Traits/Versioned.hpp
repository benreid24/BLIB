#ifndef BLIB_ECS_TRAITS_VERSIONED_HPP
#define BLIB_ECS_TRAITS_VERSIONED_HPP

#include <cstdint>

namespace bl
{
namespace ecs
{
namespace trait
{
/**
 * @brief Component trait for versions. Versions are a more robust dirty state, allowing observers
 *        of the component to independently identify if they need to refresh or not
 *
 * @ingroup ECS
 */
class Versioned {
public:
    /**
     * @brief Initializes the version to 0
     */
    Versioned()
    : version(0) {}

    /**
     * @brief Default copy constructor
     */
    Versioned(const Versioned&) = default;

    /**
     * @brief Default move constructor
     */
    Versioned(Versioned&&) = default;

    /**
     * @brief Default copy assignment
     */
    Versioned& operator=(const Versioned&) = default;

    /**
     * @brief Default move assignment
     */
    Versioned& operator=(Versioned&&) = default;

    /**
     * @brief Returns the current version of the component. The value itself is not important, only
     *        changes in the value
     */
    std::uint16_t getVersion() const { return version; }

    /**
     * @brief Increments the version. Call when the component changes
     */
    void incrementVersion() { ++version; }

private:
    std::uint16_t version;
};

} // namespace trait
} // namespace ecs
} // namespace bl

#endif
