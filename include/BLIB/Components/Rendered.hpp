#ifndef BLIB_COMPONENTS_RENDERED_HPP
#define BLIB_COMPONENTS_RENDERED_HPP

#include <BLIB/Render/Components/DrawableBase.hpp>

namespace bl
{
namespace com
{
/**
 * @brief Component that can be added to entities to trigger the Renderer to render them
 *
 * @ingroup Components
 */
class Rendered {
public:
    /**
     * @brief Creates the component
     *
     * @param entity The entity that owns this component
     * @param component The Drawable component to use for rendering
     * @param scene The scene to add to
     * @param updateSpeed The expected frequency of descriptor updates for this entity
     */
    Rendered(ecs::Entity entity, rc::rcom::DrawableBase& component, rc::Scene* scene,
             rc::UpdateSpeed updateSpeed)
    : entity(entity)
    , component(&component)
    , scene(scene)
    , updateSpeed(updateSpeed) {}

    /**
     * @brief Returns the entity rendered by this component
     */
    ecs::Entity getEntity() const { return entity; }

    /**
     * @brief Returns the drawable component used for rendering
     */
    rc::rcom::DrawableBase* getComponent() const { return component; }

    /**
     * @brief Returns the scene to be rendered in
     */
    rc::Scene* getScene() const { return scene; }

    /**
     * @brief Returns the update speed for this entity
     */
    rc::UpdateSpeed getUpdateSpeed() const { return updateSpeed; }

private:
    ecs::Entity entity;
    rc::rcom::DrawableBase* component;
    rc::Scene* scene;
    rc::UpdateSpeed updateSpeed;

    void invalidate() { scene = nullptr; }

    friend class rc::scene::SceneSync;
};

} // namespace com
} // namespace bl

#endif
