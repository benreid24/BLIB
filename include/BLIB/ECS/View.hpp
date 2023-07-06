#ifndef BLIB_ECS_VIEW_HPP
#define BLIB_ECS_VIEW_HPP

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/ComponentSet.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Events/Listener.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <array>
#include <limits>
#include <queue>
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
    const ComponentMask::Value mask;

    virtual ~ViewBase() = default;

protected:
    bool needsAddressReload;

    ViewBase(ComponentMask::Value mask)
    : mask(mask)
    , needsAddressReload(false) {}

    virtual void removeEntity(Entity entity) = 0;
    virtual void tryAddEntity(Entity entity) = 0;
    virtual void clearAndRefresh()           = 0;

    friend class Registry;
};
} // namespace priv

/**
 * @brief A view into the ECS providing fast iteration over entities with the given set of
 *        components. View results stay up to date as the component and entity data change
 *
 * @tparam TComponents The components that must all be present in the results
 * @ingroup ECS
 */
template<typename... TComponents>
class View : public priv::ViewBase {
public:
    /**
     * @brief Destroy the View object
     *
     */
    virtual ~View() = default;

    /**
     * @brief Iterates over all results of the view
     *
     * @tparam TCallback The callback type to invoke
     * @param cb Handler for each entity result in the view
     */
    template<typename TCallback>
    void forEach(const TCallback& cb);

private:
    static constexpr std::size_t InvalidIndex = std::numeric_limits<std::size_t>::max();

    Registry& registry;
    util::ReadWriteLock viewLock;
    std::array<ComponentPoolBase*, sizeof...(TComponents)> pools;
    std::vector<ComponentSet<TComponents...>> results;
    std::vector<std::size_t> entityToIndex;

    std::mutex queueLock;
    std::vector<Entity> toAdd;
    std::vector<Entity> toRemove;

    View(Registry& reg, ComponentMask::Value mask);
    virtual void removeEntity(Entity entity) override;
    virtual void tryAddEntity(Entity entity) override;
    virtual void clearAndRefresh() override;

    void ensureUpdated();

    void lockPoolsRead();
    void unlockPoolsRead();
    void lockWrite();
    void unlockWrite();

    friend class Registry;
};

} // namespace ecs
} // namespace bl

#endif
