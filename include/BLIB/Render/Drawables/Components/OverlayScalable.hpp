#ifndef BLIB_RENDER_DRAWABLES_COMPONENTS_OVERLAYSCALABLE_HPP
#define BLIB_RENDER_DRAWABLES_COMPONENTS_OVERLAYSCALABLE_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Components/OverlayScaler.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Drawables/Components/Transform2D.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>

namespace bl
{
namespace gfx
{
class Overlay;

namespace sys
{
class OverlayScaler;
}

namespace draw
{
namespace base
{
/**
 * @brief Base component that manages and provides a Transform2D, OverlayScaler, and Viewport
 */
class OverlayScalable : public Transform2D {
public:
    /**
     * @brief Does nothing
     */
    OverlayScalable();

    /**
     * @brief Returns the OverlayScaler of this entity
     */
    com::OverlayScaler& getOverlayScaler();

    /**
     * @brief Returns the pre-transform size of this entity
     */
    const glm::vec2& getLocalSize() const;

    /**
     * @brief Returns the post-transform size in Overlay space
     */
    glm::vec2 getOverlaySize() const;

    /**
     * @brief Returns the post-transform and post-viewport size in target space
     */
    glm::vec2 getTargetSize() const;

    /**
     * @brief Returns the viewport this entity is rendered to in target space
     */
    const sf::FloatRect& getTargetRegion() const;

    /**
     * @brief Creates or updates the viewport of this drawable
     *
     * @param viewport The viewport to constrain rendering to
     */
    void setViewport(const ovy::Viewport& viewport);

    /**
     * @brief Helper method to set the viewport to the region the drawable is in
     *
     * @param setToSelf True to create and use a viewport, false to render normally
     */
    void setViewportToSelf(bool setToSelf = true);

    /**
     * @brief Removes any viewport for this drawable
     */
    void clearViewport();

protected:
    /**
     * @brief Creates the OverlayScaler and Transform2D in the ECS
     *
     * @tparam ...TArgs Argument types to the Transform2D constructor
     * @param engine The game engine instance
     * @param entity The entity id
     * @param overlay Pointer to the Overlay that the object is in
     * @param sceneId The id of the entity in the scene that it is in
     * @param ...args Arguments to the Transform2D constructor
     */
    template<typename... TArgs>
    void create(engine::Engine& engine, ecs::Entity entity, TArgs&&... args);

    /**
     * @brief Sets the pre-transform size of this entity
     *
     * @param size The pre-transform size to use
     */
    void setLocalSize(const glm::vec2& size);

private:
    sys::OverlayScaler* scalerSystem;
    ecs::Registry* registry;
    ecs::Entity ecsId;
    ecs::StableHandle<com::OverlayScaler> handle;

    friend class sys::OverlayScaler;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::OverlayScaler& OverlayScalable::getOverlayScaler() { return handle.get(); }

template<typename... TArgs>
void OverlayScalable::create(engine::Engine& engine, ecs::Entity entity, TArgs&&... args) {
    scalerSystem = &engine.systems().getSystem<sys::OverlayScaler>();
    registry     = &engine.ecs();
    ecsId        = entity;

    Transform2D::create(*registry, entity, std::forward<TArgs>(args)...);
    registry->emplaceComponent<com::OverlayScaler>(entity);
    handle.assign(*registry, entity);
}

inline void OverlayScalable::setLocalSize(const glm::vec2& size) {
    handle.get().setEntitySize(size);
}

inline const glm::vec2& OverlayScalable::getLocalSize() const {
    return handle.get().getEntitySize();
}

inline const sf::FloatRect& OverlayScalable::getTargetRegion() const {
    return handle.get().cachedTargetRegion;
}

} // namespace base
} // namespace draw
} // namespace gfx
} // namespace bl

#endif
