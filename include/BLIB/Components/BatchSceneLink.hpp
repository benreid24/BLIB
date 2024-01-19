#ifndef BLIB_COMPONENTS_BATCHSCENELINK_HPP
#define BLIB_COMPONENTS_BATCHSCENELINK_HPP

#include <BLIB/ECS/Traits/ChildAware.hpp>
#include <BLIB/Render/Scenes/Key.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component to help facilitate quick lookups internally in batched scenes
 *
 * @ingroup Components
 */
struct BatchSceneLink : public ecs::trait::ChildAware<BatchSceneLink> {
    rc::scene::Key key;

    /**
     * @brief Creates a new link component
     *
     * @param key The scene key of this entity
     */
    BatchSceneLink(rc::scene::Key key)
    : key(key) {}
};

} // namespace com
} // namespace bl

#endif
