#ifndef BLIB_RENDER_SYSTEMS_GENERICDRAWABLESYSTEM_HPP
#define BLIB_RENDER_SYSTEMS_GENERICDRAWABLESYSTEM_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Events/SceneDestroyed.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObject.hpp>
#include <BLIB/Render/Scenes/StagePipelines.hpp>
#include <mutex>
#include <vector>

namespace bl
{
namespace render
{
namespace sys
{
/**
 * @brief Base class for renderer systems that manage scene objects in the ECS. Custom renderable
 *        types should have a system that inherits from this class that manages their membership
 *        within scenes. These systems should run in the engine::FrameStage::RenderObjectSync step
 *
 * @tparam T The component type in the ECS that is being managed
 * @ingroup Renderer
 */
template<typename T>
class GenericDrawableSystem
: public engine::System
, public bl::event::Listener<ecs::event::ComponentRemoved<T>, render::event::SceneDestroyed> {
public:
    /**
     * @brief Initializes the system internals
     *
     * @param pipelines The pipelines to use by default for rendering these types of objects
     */
    GenericDrawableSystem(const scene::StagePipelines& pipelines);

    /**
     * @brief Destroys the system
     */
    virtual ~GenericDrawableSystem() = default;

    /**
     * @brief Adds the component for the given entity to the scene using the default pipelines for
     *        this type of object. Component must already exist in the ECS
     *
     * @param entity The entity to add to the scene
     * @param scene The scene to add the entity to
     * @param descriptorUpdateFreq Expected update frequency of descriptors for this entity
     */
    void addToScene(ecs::Entity entity, Scene* scene, UpdateSpeed descriptorUpdateFreq);

    /**
     * @brief Adds the component for the given entity to the scene using custom pipelines for that
     *        specific object
     *
     * @param entity The entity to add to the scene
     * @param scene The scene to add the entity to
     * @param descriptorUpdateFreq Expected update frequency of descriptors for this entity
     * @param pipelines The pipelines to use for each scene stage when rendering this object
     */
    void addToSceneWithCustomPipelines(ecs::Entity entity, Scene* scene,
                                       UpdateSpeed descriptorUpdateFreq,
                                       const scene::StagePipelines& pipelines);

    /**
     * @brief Removes the entity from the scene that it is currently in
     *
     * @param entity The entity to remove from the scene
     */
    void removeFromScene(ecs::Entity entity);

protected:
    /**
     * @brief Does nothing. Derived classes should override this if they need custom init logic
     *
     * @param engine The game engine instance
     */
    virtual void doInit(engine::Engine& engine);

    /**
     * @brief Does nothing. Derived classes should override this if they need custom update logic
     *
     * @param mutex Shared mutex for other systems running in parallel in the same frame stage
     * @param dt Time elapsed in seconds
     */
    virtual void doUpdate(std::mutex& mutex, float dt);

private:
    struct AddCommand {
        ecs::Entity entity;
        Scene* scene;
        UpdateSpeed updateFreq;
        scene::StagePipelines pipelines;

        AddCommand(ecs::Entity ent, Scene* s, UpdateSpeed us,
                   const scene::StagePipelines& pipelines)
        : entity(ent)
        , scene(s)
        , updateFreq(us)
        , pipelines(pipelines) {}
    };

    std::mutex mutex;
    ecs::Registry* registry;
    scene::StagePipelines stagePipelines;
    std::vector<AddCommand> toAdd;
    std::vector<ecs::Entity> toRemove;
    std::vector<com::SceneObjectRef> erased;

    virtual void observe(const ecs::event::ComponentRemoved<T>& rm) override;
    virtual void observe(const event::SceneDestroyed& rm) override;
    virtual void init(engine::Engine& engine);
    virtual void update(std::mutex& mutex, float dt) override;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

template<typename T>
GenericDrawableSystem<T>::GenericDrawableSystem(const scene::StagePipelines& pipelines)
: registry(nullptr)
, stagePipelines(pipelines) {
    toAdd.reserve(64);
    toRemove.reserve(64);
    erased.reserve(64);
}

template<typename T>
void GenericDrawableSystem<T>::addToScene(ecs::Entity ent, Scene* scene,
                                          UpdateSpeed descriptorUpdateFreq) {
    addToSceneWithCustomPipelines(ent, scene, descriptorUpdateFreq, stagePipelines);
}

template<typename T>
void GenericDrawableSystem<T>::addToSceneWithCustomPipelines(
    ecs::Entity entity, Scene* scene, UpdateSpeed descriptorUpdateFreq,
    const scene::StagePipelines& pipelines) {
    std::unique_lock lock(mutex);
    T* c = registry->getComponent<T>(entity);
    if (!c) {
#ifdef BLIB_DEBUG
        BL_LOG_WARN << "Entity " << entity << " is missing component: " << typeid(T).name();
#endif
        return;
    }
    if (c->sceneRef.scene) {
        if (c->sceneRef.scene == scene) { return; }
        toRemove.emplace_back(entity);
    }
    toAdd.emplace_back(entity, scene, descriptorUpdateFreq, pipelines);
}

template<typename T>
void GenericDrawableSystem<T>::doInit(engine::Engine&) {}

template<typename T>
void GenericDrawableSystem<T>::doUpdate(std::mutex&, float) {}

template<typename T>
void GenericDrawableSystem<T>::removeFromScene(ecs::Entity ent) {
    std::unique_lock lock(mutex);
    toRemove.emplace_back(ent);
}

template<typename T>
void GenericDrawableSystem<T>::observe(const ecs::event::ComponentRemoved<T>& rm) {
    std::unique_lock lock(mutex);
    erased.emplace_back(rm.component.sceneRef);
}

template<typename T>
void GenericDrawableSystem<T>::observe(const event::SceneDestroyed& rm) {
    registry->getAllComponents<T>().forEach([&rm](ecs::Entity, T& c) {
        if (c.sceneRef.scene == rm.scene) { c.sceneRef.scene = nullptr; }
    });
}

template<typename T>
void GenericDrawableSystem<T>::init(engine::Engine& engine) {
    registry = &engine.ecs();
    bl::event::Dispatcher::subscribe(this);
    doInit(engine);
}

template<typename T>
void GenericDrawableSystem<T>::update(std::mutex& frameMutex, float dt) {
    if (!toAdd.empty() || !toRemove.empty() || !erased.empty()) {
        std::unique_lock lock(frameMutex);

        for (ecs::Entity ent : toRemove) {
            T* c = registry->getComponent<T>(ent);
            if (!c) {
#ifdef BLIB_DEBUG
                BL_LOG_WARN << "Entity erased before it could be removed from scene";
#endif
                continue;
            }
            c->sceneRef.scene->removeObject(ent);
        }
        toRemove.clear();

        for (const com::SceneObjectRef& ref : erased) { ref.scene->removeObject(ref.object); }
        erased.clear();

        for (const auto& add : toAdd) {
            T* c = registry->getComponent<T>(add.entity);
            if (!c) {
#ifdef BLIB_DEBUG
                BL_LOG_WARN << "Entity erased before it could be added to scene";
#endif
                continue;
            }
            c->sceneRef.object = add.scene->createAndAddObject(
                add.entity, c->drawParams, add.updateFreq, add.pipelines);
            if (c->sceneRef.object) { c->sceneRef.scene = add.scene; }
            else {
                BL_LOG_WARN << "Failed to add entity " << add.entity << " to scene " << add.scene;
            }
        }
        toAdd.clear();
    }

    doUpdate(frameMutex, dt);
}

} // namespace sys
} // namespace render
} // namespace bl

#endif
