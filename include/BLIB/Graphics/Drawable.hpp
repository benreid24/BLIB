#ifndef BLIB_GRAPHICS_DRAWABLE_HPP
#define BLIB_GRAPHICS_DRAWABLE_HPP

#include <BLIB/Components/Toggler.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Graphics/Components/EntityBacked.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Systems/DrawableSystem.hpp>
#include <type_traits>

namespace bl
{
namespace engine
{
class Engine;
}

/// Collection of SFML-like classes for drawing
namespace gfx
{
template<typename TCom, typename TSys>
class Drawable;

/**
 * @brief Base class for Drawable objects. A Drawable object should inherit this class for the
 *        drawable component that it wraps
 *
 * @tparam TCom The drawable component type (ie Sprite)
 * @tparam TSys The drawable system. Must implement DrawableSystem interface
 * @ingroup Graphics
 */
template<typename TCom, typename TSys = sys::DrawableSystem<TCom>>
class Drawable : public bcom::EntityBacked {
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
     * @brief Stops the component from flashing
     */
    void stopFlashing();

    /**
     * @brief Helper method to set the scale so that the entity is a certain size
     *
     * @param size The size to scale to
     */
    virtual void scaleToSize(const glm::vec2& size) = 0;

protected:
    /**
     * @brief Initializes the drawable
     */
    Drawable();

    /**
     * @brief Returns the drawable system. Only call after create()
     */
    TSys& system();

    /**
     * @brief Creates the ECS entity and drawable component. Will destroy the prior instance if any
     *
     * @tparam ...TArgs Argument types to the component's constructor
     * @param engine Game engine instance
     * @param ...args Arguments to the component's constructor
     */
    template<typename... TArgs>
    void create(engine::Engine& engine, TArgs&&... args);

    /**
     * @brief Creates the component for the drawable. Must only be called after entity creation
     *
     * @tparam ...TArgs Argument types to the components constructor
     * @param ...args Arguments to the components constructor
     */
    template<typename... TArgs>
    void createComponentOnly(TArgs&&... args);

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

    template<typename U, typename US>
    friend class Drawable;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TSys>
Drawable<TCom, TSys>::Drawable()
: handle(nullptr) {}

template<typename TCom, typename TSys>
Drawable<TCom, TSys>::~Drawable() {
    if (entity() != ecs::InvalidEntity) { removeFromScene(); }
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::addToScene(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
#ifdef BLIB_DEBUG
    if (entity() == ecs::InvalidEntity || !handle) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    system().addToScene(entity(), scene, updateFreq);
    onAdd(component().sceneRef);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::addToSceneWithCustomPipeline(rc::Scene* scene,
                                                        rc::UpdateSpeed updateFreq,
                                                        std::uint32_t pipeline) {
#ifdef BLIB_DEBUG
    if (entity() == ecs::InvalidEntity || !handle) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    system().addToSceneWithCustomPipeline(entity(), scene, updateFreq, pipeline);
    onAdd(component().sceneRef);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::setHidden(bool hide) {
#ifdef BLIB_DEBUG
    if (!handle) { throw std::runtime_error("Cannot hide un-created entity"); }
#endif

    component().setHidden(hide);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::removeFromScene() {
#ifdef BLIB_DEBUG
    if (entity() == ecs::InvalidEntity || !handle) {
        throw std::runtime_error("Drawable must be created before removing from scene");
    }
#endif

    system().removeFromScene(entity());
    onRemove();
}

template<typename TCom, typename TSys>
TCom& Drawable<TCom, TSys>::component() {
    return *handle;
}

template<typename TCom, typename TSys>
const TCom& Drawable<TCom, TSys>::component() const {
    return *handle;
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::flash(float onPeriod, float offPeriod) {
    // swap off/on period because we have hidden toggle instead of visible toggle
    engine().ecs().emplaceComponent<com::Toggler>(
        entity(), offPeriod, onPeriod, &component().sceneRef.object->hidden);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::stopFlashing() {
    engine().ecs().removeComponent<com::Toggler>(entity());
    component().sceneRef.object->hidden = false;
}

template<typename TCom, typename TSys>
TSys& Drawable<TCom, TSys>::system() {
    return engine().systems().getSystem<TSys>();
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::onAdd(const rc::rcom::SceneObjectRef&) {}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::onRemove() {}

template<typename TCom, typename TSys>
template<typename... TArgs>
void Drawable<TCom, TSys>::createComponentOnly(TArgs&&... args) {
    handle = engine().ecs().emplaceComponent<TCom>(entity(), std::forward<TArgs>(args)...);
}

template<typename TCom, typename TSys>
template<typename... TArgs>
void Drawable<TCom, TSys>::create(engine::Engine& engine, TArgs&&... args) {
    if (handle != nullptr) { destroy(); }
    createEntityOnly(engine);
    createComponentOnly(std::forward<TArgs>(args)...);
}

} // namespace gfx
} // namespace bl

#endif
