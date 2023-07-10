#ifndef BLIB_ECS_VIEW_HPP
#define BLIB_ECS_VIEW_HPP

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/ComponentSet.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/ECS/Tags.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Events/Listener.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <array>
#include <limits>
#include <queue>
#include <typeindex>
#include <vector>

namespace bl
{
namespace ecs
{
class Registry;

/// Internal implementation
namespace priv
{
/**
 * @brief Helper class not to be used directly
 *
 * @ingroup ECS
 */
class ViewBase : private util::NonCopyable {
public:
    const std::type_index id;
    const ComponentMask mask;

    virtual ~ViewBase() = default;

protected:
    bool needsAddressReload;

    ViewBase(ComponentMask mask, std::type_index id)
    : id(id)
    , mask(mask)
    , needsAddressReload(false) {}

    virtual void removeEntity(Entity entity) = 0;
    virtual void tryAddEntity(Entity entity) = 0;
    virtual void clearAndRefresh()           = 0;

    friend class bl::ecs::Registry;
};
} // namespace priv

/**
 * @brief Base definition of the View class. Use the tagged specialization
 *
 * @tparam TRequire Required tagged components. ie Require<int, char>
 * @tparam TOptional Optional tagged components. ie Optional<bool>
 * @tparam TExclude Excluded tagged components. ie Exclude<std::string, unsigned>
 * @ingroup ECS
 */
template<typename TRequire, typename TOptional = Optional<>, typename TExclude = Exclude<>>
class View : public priv::ViewBase {
    static_assert(std::is_same_v<TRequire, void>,
                  "View must use tags. Example: View<Require<int, char>, "
                  "Optional<std::string>, Exclude<unsigned>>");
};

} // namespace ecs
} // namespace bl

#endif
