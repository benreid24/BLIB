#ifndef BLIB_ECS_TRAITS_PARENTAWAREVERSIONED_HPP
#define BLIB_ECS_TRAITS_PARENTAWAREVERSIONED_HPP

#include <BLIB/ECS/Traits/ParentAware.hpp>
#include <BLIB/ECS/Traits/Versioned.hpp>
#include <limits>

namespace bl
{
namespace ecs
{
namespace trait
{
/**
 * @brief Combined trait that is both parent aware and versioned. Maintains an internal dirty state
 *        that derived classes can use to determine if they need to refresh themselves. The dirty
 *        state is recursively defined with the entire parent tree
 *
 * @tparam T The derived component type
 * @ingroup ECS
 */
template<typename T>
class ParentAwareVersioned
: public ParentAware<T>
, public Versioned {
public:
    /**
     * @brief Creates the trait and registers as dirty
     */
    ParentAwareVersioned()
    : lastRefreshVersion(std::numeric_limits<std::uint16_t>::max())
    , lastParentVersion(std::numeric_limits<std::uint16_t>::max())
    , lastParent(nullptr) {}

    /**
     * @brief Default copy constructor
     */
    ParentAwareVersioned(const ParentAwareVersioned&) = default;

    /**
     * @brief Default move constructor
     */
    ParentAwareVersioned(ParentAwareVersioned&&) = default;

    /**
     * @brief Default copy assignment
     */
    ParentAwareVersioned& operator=(const ParentAwareVersioned&) = default;

    /**
     * @brief Default move assignment
     */
    ParentAwareVersioned& operator=(ParentAwareVersioned&&) = default;

    /**
     * @brief Returns whether or not this component needs to be refreshed. Takes into account parent
     *        state as well, including parent changes
     */
    bool refreshRequired() const {
        if (lastRefreshVersion != getVersion()) { return true; }
        if (this->hasParent()) {
            auto& parent = this->getParent();
            if (lastParent != &parent) { return true; }
            if (parent.getVersion() != lastParentVersion) { return true; }
            return parent.refreshRequired();
        }
        return lastParent != nullptr;
    }

protected:
    /**
     * @brief Marks the component as refreshed
     */
    void markRefreshed() {
        lastRefreshVersion = getVersion();
        if (this->hasParent()) {
            lastParentVersion = this->getParent().getVersion();
            lastParent        = &this->getParent();
        }
        else { lastParent = nullptr; }
    }

private:
    std::uint16_t lastRefreshVersion;
    std::uint16_t lastParentVersion;
    ParentAwareVersioned* lastParent;
};

} // namespace trait
} // namespace ecs
} // namespace bl

#endif
