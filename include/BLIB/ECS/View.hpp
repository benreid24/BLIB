#ifndef BLIB_ECS_VIEW_HPP
#define BLIB_ECS_VIEW_HPP

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/ComponentSet.hpp>
#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Util/NonCopyable.hpp>
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

protected:
    ViewBase(ComponentMask::Value mask)
    : mask(mask) {}

    virtual void removeEntity(Entity entity)         = 0;
    virtual void tryAddEntity(Entity entity)         = 0;
    virtual void clearAndRefresh(Registry& registry) = 0;

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
     * @brief The beginning of the result set of this view
     *
     * @return std::vector<ComponentSet<TComponents...>>::iterator The start of the results
     */
    std::vector<ComponentSet<TComponents...>>::iterator begin();

    /**
     * @brief The end of the result set of this view
     *
     * @return std::vector<ComponentSet<TComponents...>>::iterator The end of the results
     */
    std::vector<ComponentSet<TComponents...>>::iterator end();

private:
    std::vector<ComponentSet<TComponents...>> results;

    View(std::size_t maxEntityCount);
    virtual void removeEntity(Entity entity) override;
    virtual void tryAddEntity(Entity entity) override;
    virtual void clearAndRefresh(Registry& registry) override;

    friend class Registry;
};

} // namespace ecs
} // namespace bl

#endif
