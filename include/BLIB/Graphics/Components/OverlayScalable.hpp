#ifndef BLIB_GRAPHICS_COMPONENTS_OVERLAYSCALABLE_HPP
#define BLIB_GRAPHICS_COMPONENTS_OVERLAYSCALABLE_HPP

#include <BLIB/Components/OverlayScaler.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Graphics/Components/Transform2D.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>

namespace bl
{
namespace sys
{
class OverlayScalerSystem;
}
namespace rc
{
class Overlay;
}

namespace gfx
{
namespace bcom
{
/**
 * @brief Base component that manages and provides a Transform2D, OverlayScaler, and Viewport
 *
 * @ingroup Graphics
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
     * @brief Returns the parent bounds in overlay space
     */
    const sf::FloatRect& getTargetRegion() const;

    /**
     * @brief Helper method to set the viewport to the region the drawable is in
     *
     * @param setToSelf True to create and use a viewport, false to render normally
     */
    void setScissorToSelf(bool setToSelf = true);

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

    /**
     * @brief Called when the local size is queried
     */
    virtual void ensureLocalSizeUpdated() = 0;

private:
    sys::OverlayScalerSystem* scalerSystem;
    ecs::Registry* registry;
    ecs::Entity ecsId;
    com::OverlayScaler* handle;

    friend class sys::OverlayScalerSystem;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline com::OverlayScaler& OverlayScalable::getOverlayScaler() { return *handle; }

template<typename... TArgs>
void OverlayScalable::create(engine::Engine& engine, ecs::Entity entity, TArgs&&... args) {
    scalerSystem = &engine.systems().getSystem<sys::OverlayScalerSystem>();
    registry     = &engine.ecs();
    ecsId        = entity;

    Transform2D::create(*registry, entity, std::forward<TArgs>(args)...);
    handle = registry->emplaceComponent<com::OverlayScaler>(entity);
}

inline void OverlayScalable::setLocalSize(const glm::vec2& size) { handle->setEntitySize(size); }

inline const sf::FloatRect& OverlayScalable::getTargetRegion() const {
    return handle->cachedTargetRegion;
}

} // namespace bcom
} // namespace gfx
} // namespace bl

#endif
