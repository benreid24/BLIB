#ifndef BLIB_ECS_COMPONENTSET_HPP
#define BLIB_ECS_COMPONENTSET_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Tags.hpp>

namespace bl
{
namespace ecs
{
class Registry;

namespace priv
{
/**
 * @brief Helper class to compose component sets
 *
 * @tparam T The single component type to make a member
 * @tparam Optional Whether or not this component is optional
 * @ingroup ECS
 */
template<typename T>
struct ComponentSetMember {
    T* component;
    ComponentSetMember();
    bool populate(Registry& registry, Entity owner);
};
} // namespace priv

/**
 * @brief Contains a set of components for a single entity. May not always be valid. Does not update
 *        if components are added or removed from the entity. Sets from views are always valid
 *
 * @tparam TRequire Required tagged components. ie Require<int, char>
 * @tparam TOptional Optional tagged components. ie Optional<bool>
 * @ingroup ECS
 */
template<typename TRequire, typename TOptional = Optional<>>
class ComponentSet {
    static_assert(std::is_same_v<TRequire, void>,
                  "ComponentSet requires tagged types: ComponentSet<Require<int, char>, "
                  "Optional<std::string>>");
};

} // namespace ecs
} // namespace bl

#endif
