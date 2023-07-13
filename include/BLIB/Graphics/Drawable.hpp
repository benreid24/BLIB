#ifndef BLIB_GRAPHICS_DRAWABLE_HPP
#define BLIB_GRAPHICS_DRAWABLE_HPP

#include <BLIB/ECS.hpp>
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
/**
 * @brief Base class for Drawable objects. A Drawable object should inherit this class for the
 *        drawable component that it wraps
 *
 * @tparam TCom The drawable component type (ie Sprite)
 * @tparam TSys The drawable system. Must implement DrawableSystem interface
 * @ingroup Renderer
 */
template<typename TCom, typename TSys = sys::DrawableSystem<TCom>>
class Drawable {
    static_assert(std::is_base_of_v<rc::rcom::DrawableBase, TCom>,
                  "TCom must derive from DrawableBase");

public:
    /**
     * @brief Destroys the entity
     */
    virtual ~Drawable();

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
     * @brief Adds this entity to the given overlay
     *
     * @param overlay The overlay to add to
     * @param descriptorUpdateFreq Whether the entity is expected to be dynamic or static
     * @param parent The parent entity or InvalidEntity to make a root
     */
    void addToOverlay(rc::Overlay* overlay, rc::UpdateSpeed descriptorUpdateFreq,
                      ecs::Entity parent = ecs::InvalidEntity);

    /**
     * @brief Adds this entity to the given overlay with custom pipelines
     *
     * @param overlay The scene to add to
     * @param descriptorUpdateFreq Whether the entity is expected to be dynamic or static
     * @param pipeline The pipeline to render with
     * @param parent The parent entity or InvalidEntity to make a root
     */
    void addToOverlayWithCustomPipeline(rc::Overlay* overlay, rc::UpdateSpeed descriptorUpdateFreq,
                                        std::uint32_t pipeline,
                                        ecs::Entity parent = ecs::InvalidEntity);

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
     * @brief Returns the ECS entity of this drawable
     */
    constexpr ecs::Entity entity() const;

    /**
     * @brief Destroys the ECS entity and component
     */
    void destroy();

    /**
     * @brief Returns the drawable component. Only call after create()
     */
    TCom& component();

    /**
     * @brief Returns the drawable component. Only call after create()
     */
    const TCom& component() const;

protected:
    /**
     * @brief Initializes the drawable
     */
    Drawable();

    /**
     * @brief Returns the game engine instance. Only call after create()
     */
    constexpr engine::Engine& engine();

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
    engine::Engine* enginePtr;
    ecs::Entity ecsId;
    ecs::StableHandle<TCom> handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename TCom, typename TSys>
Drawable<TCom, TSys>::Drawable()
: enginePtr(nullptr)
, ecsId(ecs::InvalidEntity) {}

template<typename TCom, typename TSys>
Drawable<TCom, TSys>::~Drawable() {
    if (enginePtr && ecsId != ecs::InvalidEntity) {
        onRemove();
        enginePtr->ecs().destroyEntity(ecsId);
    }
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::addToScene(rc::Scene* scene, rc::UpdateSpeed updateFreq) {
#ifdef BLIB_DEBUG
    if (!enginePtr || ecsId == ecs::InvalidEntity || !handle.valid()) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    system().addToScene(ecsId, scene, updateFreq);
    onAdd(component().sceneRef);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::addToSceneWithCustomPipeline(rc::Scene* scene,
                                                        rc::UpdateSpeed updateFreq,
                                                        std::uint32_t pipeline) {
#ifdef BLIB_DEBUG
    if (!enginePtr || ecsId == ecs::InvalidEntity || !handle.valid()) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    system().addToSceneWithCustomPipeline(ecsId, scene, updateFreq, pipeline);
    onAdd(component().sceneRef);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::addToOverlay(rc::Overlay* overlay, rc::UpdateSpeed descriptorUpdateFreq,
                                        ecs::Entity parent) {
#ifdef BLIB_DEBUG
    if (!enginePtr || ecsId == ecs::InvalidEntity || !handle.valid()) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    system().addToOverlay(ecsId, overlay, descriptorUpdateFreq, parent);
    onAdd(component().sceneRef);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::addToOverlayWithCustomPipeline(rc::Overlay* overlay,
                                                          rc::UpdateSpeed descriptorUpdateFreq,
                                                          std::uint32_t pipeline,
                                                          ecs::Entity parent) {
#ifdef BLIB_DEBUG
    if (!enginePtr || ecsId == ecs::InvalidEntity || !handle.valid()) {
        throw std::runtime_error("Drawable must be created before adding to scene");
    }
#endif

    system().addToOverlayWithCustomPipeline(ecsId, overlay, descriptorUpdateFreq, pipeline, parent);
    onAdd(component().sceneRef);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::setHidden(bool hide) {
#ifdef BLIB_DEBUG
    if (!handle.valid()) { throw std::runtime_error("Cannot hide un-created entity"); }
#endif

    component().setHidden(hide);
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::removeFromScene() {
#ifdef BLIB_DEBUG
    if (!enginePtr) {
        throw std::runtime_error("Drawable must be created before removing from scene");
    }
#endif

    system().removeFromScene(ecsId);
    onRemove();
}

template<typename TCom, typename TSys>
constexpr ecs::Entity Drawable<TCom, TSys>::entity() const {
    return ecsId;
}

template<typename TCom, typename TSys>
TCom& Drawable<TCom, TSys>::component() {
    return handle.get();
}

template<typename TCom, typename TSys>
const TCom& Drawable<TCom, TSys>::component() const {
    return handle.get();
}

template<typename TCom, typename TSys>
constexpr engine::Engine& Drawable<TCom, TSys>::engine() {
    return *enginePtr;
}

template<typename TCom, typename TSys>
TSys& Drawable<TCom, TSys>::system() {
#ifdef BLIB_DEBUG
    if (!enginePtr) {
        throw std::runtime_error("Drawable must be created before accessing system");
    }
#endif

    return enginePtr->systems().getSystem<TSys>();
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::destroy() {
#ifdef BLIB_DEBUG
    if (!enginePtr) { throw std::runtime_error("Drawable must be created before being destroyed"); }
#endif

    enginePtr->ecs().destroyEntity(ecsId);
    ecsId = ecs::InvalidEntity;
    handle.release();
}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::onAdd(const rc::rcom::SceneObjectRef&) {}

template<typename TCom, typename TSys>
void Drawable<TCom, TSys>::onRemove() {}

template<typename TCom, typename TSys>
template<typename... TArgs>
void Drawable<TCom, TSys>::create(engine::Engine& engine, TArgs&&... args) {
    if (handle.valid()) { destroy(); }
    enginePtr = &engine;

    ecsId = engine.ecs().createEntity();
    engine.ecs().emplaceComponent<TCom>(ecsId, std::forward<TArgs>(args)...);
    handle.assign(engine.ecs(), ecsId);
}

} // namespace gfx
} // namespace bl

#endif
