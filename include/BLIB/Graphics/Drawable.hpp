#ifndef BLIB_GRAPHICS_DRAWABLE_HPP
#define BLIB_GRAPHICS_DRAWABLE_HPP

#include <BLIB/Components/Toggler.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/ECS/EntityBacked.hpp>
#include <BLIB/Engine.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Scenes/CodeScene.hpp>
#include <BLIB/Resources/State.hpp>
#include <type_traits>

namespace bl
{
namespace engine
{
class Engine;
class World;
} // namespace engine

/// Collection of SFML-like classes for drawing
namespace gfx
{
template<typename TCom>
class Drawable;

/**
 * @brief Base class for Drawable objects. A Drawable object should inherit this class for the
 *        drawable component that it wraps
 *
 * @tparam TCom The drawable component type (ie Sprite)
 * @ingroup Graphics
 */
template<typename TCom>
class Drawable : public ecs::EntityBacked {
    static_assert(std::is_base_of_v<rc::rcom::DrawableBase, TCom>,
                  "TCom must derive from DrawableBase");

public:
    /**
     * @brief Assumes ownership of the underlying entity from the given drawable
     *
     * @param move The drawable to move from
     */
    Drawable(Drawable&& move) = default;

    /**
     * @brief Destroys the entity
     */
    virtual ~Drawable();

    /**
     * @brief Assumes ownership of the underlying entity from the given drawable
     *
     * @param move The drawable to move from
     * @return A reference to this object
     */
    Drawable& operator=(Drawable&& move) = default;

    /**
     * @brief Adds this entity to the given scene
     *
     * @param scene The scene to add to
     * @param updateFreq Whether the entity is expected to be dynamic or static
     */
    void addToScene(rc::Scene* scene, rc::UpdateSpeed updateFreq);

    /**
     * @brief Adds this entity to the given scene with custom pipelines
     *
     * @param scene The scene to add to
     * @param updateFreq Whether the entity is expected to be dynamic or static
     * @param pipeline The pipeline to render with
     */
    void addToSceneWithCustomPipeline(rc::Scene* scene, rc::UpdateSpeed updateFreq,
                                      std::uint32_t pipeline);

    /**
     * @brief Set whether the entity is hidden or not. May only be called on entities in a scene
     *
     * @param hide True to block rendering, false to render normally
     */
    void setHidden(bool hide);

    /**
     * @brief Removes the entity from the scene or overlay that it is in
     */
    void removeFromScene();

    /**
     * @brief Returns the drawable component. Only call after create()
     */
    TCom& component();

    /**
     * @brief Returns the drawable component. Only call after create()
     */
    const TCom& component() const;

    /**
     * @brief Causes the drawable to flash with the given settings
     *
     * @param onPeriod The amount of time to be visible, in seconds
     * @param offPeriod The amount of time to be hidden, in seconds
     */
    void flash(float onPeriod, float offPeriod);

    /**
     * @brief If actively flashing: Resets state to visible and restarts flash timer
     */
    void resetFlash();

    /**
     * @brief Stops the component from flashing
     */
    void stopFlashing();

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) = 0;

    /**
     * @brief Manually issue the required commands to draw this object. Must be called within the
     *        context of a CodeScene
     *
     * @param ctx The CodeScene rendering context
     */
    void draw(rc::scene::CodeScene::RenderContext& ctx);

protected:
    /**
     * @brief Initializes the drawable
     */
    Drawable();

    /**
     * @brief Creates the ECS entity and drawable component. Will destroy the prior instance if any
     *
     * @tparam ...TArgs Argument types to the component's constructor
     * @param world The world to create the object in
     * @param ...args Arguments to the component's constructor
     */
    template<typename... TArgs>
    void create(engine::World& world, TArgs&&... args);

    /**
     * @brief Creates the component for the drawable. Must only be called after entity creation
     *
     * @tparam ...TArgs Argument types to the components constructor
     * @param world The world to create the object in
     * @param existingEntity An already existing entity to add components to
     * @param ...args Arguments to the components constructor
     */
    template<typename... TArgs>
    void createComponentOnly(engine::World& world, ecs::Entity existingEntity, TArgs&&... args);

    /**
     * @brief Called after the entity is added to a scene. Allows derived to sync data if required
     *
     * @param sceneRef The scene object information
     */
    virtual void onAdd(const rc::rcom::SceneObjectRef& sceneRef);

    /**
     * @brief Called after the entity is removed from a scene
     */
    virtual void onRemove();

private:
    TCom* handle;

    template<typename U>
    friend class Drawable;

    void doFlash(float on, float off);
    void retryFlash(float on, float off);
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom>
Drawable<TCom>::Drawable()
: handle(nullptr) {}

template<typename TCom>
Drawable<TCom>::~Drawable() {
    if (exists() && component().getSceneRef().scene && component().getSceneRef().object &&
        entityIsDeletedOnDestruction() && !resource::State::engineShuttingDown()) {
        removeFromScene();
    }
}

template<typename TCom>
void Drawable<TCom>::addToScene(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
#ifdef BLIB_DEBUG
    if (entity() == ecs::InvalidEntity || !handle) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    component().addToScene(engine().ecs(), entity(), scene, updateFreq);
    onAdd(component().getSceneRef());
}

template<typename TCom>
void Drawable<TCom>::addToSceneWithCustomPipeline(rc::Scene* scene, rc::UpdateSpeed updateFreq,
                                                  std::uint32_t pipeline) {
#ifdef BLIB_DEBUG
    if (entity() == ecs::InvalidEntity || !handle) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    component().addToSceneWithPipeline(engine().ecs(), entity(), scene, updateFreq, pipeline);
    onAdd(component().getSceneRef());
}

template<typename TCom>
void Drawable<TCom>::setHidden(bool hide) {
#ifdef BLIB_DEBUG
    if (!handle) { throw std::runtime_error("Cannot hide un-created entity"); }
#endif

    component().setHidden(hide);
}

template<typename TCom>
void Drawable<TCom>::removeFromScene() {
#ifdef BLIB_DEBUG
    if (entity() == ecs::InvalidEntity || !handle) {
        throw std::runtime_error("Drawable must be created before removing from scene");
    }
#endif

    stopFlashing();
    component().removeFromScene(engine().ecs(), entity());
    onRemove();
}

template<typename TCom>
TCom& Drawable<TCom>::component() {
    return *handle;
}

template<typename TCom>
const TCom& Drawable<TCom>::component() const {
    return *handle;
}

template<typename TCom>
void Drawable<TCom>::flash(float onPeriod, float offPeriod) {
    if (component().getSceneRef().object) { doFlash(onPeriod, offPeriod); }
    else {
        engine().renderer().vulkanState().cleanupManager.add(
            [this, onPeriod, offPeriod]() { retryFlash(onPeriod, offPeriod); });
    }
}

template<typename TCom>
void Drawable<TCom>::retryFlash(float onPeriod, float offPeriod) {
    if (component().getSceneRef().object) { doFlash(onPeriod, offPeriod); }
    else {
        BL_LOG_WARN << "Failed to flash object (" << entity()
                    << ") Not in scene after delayed retry";
    }
}

template<typename TCom>
void Drawable<TCom>::doFlash(float onPeriod, float offPeriod) {
    // swap off/on period because we have hidden toggle instead of visible toggle
    engine().ecs().template emplaceComponent<com::Toggler>(
        entity(), offPeriod, onPeriod, &component().getSceneRef().object->hidden);
}

template<typename TCom>
void Drawable<TCom>::resetFlash() {
    com::Toggler* tog = engine().ecs().template getComponent<com::Toggler>(entity());
    if (tog) {
        tog->time   = 0.f;
        *tog->value = false;
    }
}

template<typename TCom>
void Drawable<TCom>::stopFlashing() {
    if (entity() != ecs::InvalidEntity) {
        engine().ecs().template removeComponent<com::Toggler>(entity());
        if (component().getSceneRef().object) { component().getSceneRef().object->hidden = false; }
    }
}

template<typename TCom>
void Drawable<TCom>::onAdd(const rc::rcom::SceneObjectRef&) {}

template<typename TCom>
void Drawable<TCom>::onRemove() {}

template<typename TCom>
template<typename... TArgs>
void Drawable<TCom>::createComponentOnly(engine::World& world, ecs::Entity existing,
                                         TArgs&&... args) {
    if (handle != nullptr) { destroy(); }
    createFromExistingEntity(world, existing);
    handle = engine().ecs().template emplaceComponent<TCom>(entity(), std::forward<TArgs>(args)...);
}

template<typename TCom>
template<typename... TArgs>
void Drawable<TCom>::create(engine::World& world, TArgs&&... args) {
    if (handle != nullptr) { destroy(); }
    createEntityOnly(world);
    handle = world.engine().ecs().template emplaceComponent<TCom>(entity(),
                                                                  std::forward<TArgs>(args)...);
}

template<typename TCom>
void Drawable<TCom>::draw(rc::scene::CodeScene::RenderContext& ctx) {
    ctx.renderObject(*handle);
}

} // namespace gfx
} // namespace bl

#endif
