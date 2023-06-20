#ifndef BLIB_ECS_VIEW_HPP
#define BLIB_ECS_VIEW_HPP

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/ComponentSet.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/ECS/Events.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <array>
#include <queue>

namespace bl
{
namespace ecs
{
class Registry;

/**
 * @brief Helper class not to be used directly
 *
 * @ingroup ECS
 *
 */
class ViewBase : private util::NonCopyable {
public:
    /// @brief The component mask of this view
    const ComponentMask::Value mask;

    /**
     * @brief Destroy the View Base object
     *
     */
    virtual ~ViewBase() = default;

protected:
    ViewBase(ComponentMask::Value mask)
    : mask(mask) {}

    virtual void removeEntity(Entity entity) = 0;
    virtual void tryAddEntity(Entity entity) = 0;
    virtual void clearAndRefresh()           = 0;

    friend class Registry;
};

/**
 * @brief A view into the ECS providing fast iteration over entities with the given set of
 *        components. View results stay up to date as the component and entity data change
 *
 * @tparam TComponents The components that must all be present in the results
 * @ingroup ECS
 */
template<typename... TComponents>
class View : public ViewBase {
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
    Registry& registry;
    util::ReadWriteLock viewLock;
    std::array<ComponentPoolBase*, sizeof...(TComponents)> pools;
    std::vector<ComponentSet<TComponents...>> results;
    std::vector<bool> containedEntities;

    std::mutex queueLock;
    std::vector<Entity> toAdd;
    std::vector<Entity> toRemove;

    View(Registry& reg, std::size_t maxEntityCount, ComponentMask::Value mask);
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
