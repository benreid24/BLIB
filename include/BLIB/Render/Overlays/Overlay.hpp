#ifndef BLIB_RENDER_OVERLAYS_OVERLAY_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAY_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Events/SceneObjectRemoved.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObjectECSAdaptor.hpp>
#include <BLIB/Signals/Emitter.hpp>
#include <BLIB/Signals/Listener.hpp>
#include <BLIB/Systems/OverlayScalerSystem.hpp>
#include <BLIB/Util/IdAllocator.hpp>
#include <limits>
#include <unordered_map>
#include <vector>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Renderer;
class Observer;

/**
 * @brief Special type of scene specialized for 2d overlays with fixed cameras. Objects are
 *        organized into a tree and rendered in tree order
 *
 * @ingroup Renderer
 */
class Overlay
: public Scene
, public sig::Listener<ecs::event::EntityParentSet, ecs::event::EntityParentRemoved,
                       ecs::event::ComponentRemoved<ovy::OverlayObject>> {
public:
    static constexpr std::uint32_t NoParent = std::numeric_limits<std::uint32_t>::max();

    /**
     * @brief Creates a new overlay scene
     *
     * @param engine The game engine instance
     */
    Overlay(engine::Engine& engine);

    /**
     * @brief Frees resources
     */
    virtual ~Overlay();

    /**
     * @brief Renders all objects in the overlay
     *
     * @param context Render context containing scene render data
     */
    virtual void renderOpaqueObjects(scene::SceneRenderContext& context) override;

    /**
     * @brief Does nothing
     */
    virtual void renderTransparentObjects(scene::SceneRenderContext&) override {}

    /**
     * @brief Replaces the current strategy with a new one of type T. Default is
     *        rgi::OverlayRenderStrategy
     *
     * @param strategy The new render strategy to use
     */
    static void useRenderStrategy(rg::Strategy* strategy);

    /**
     * @brief Returns the render strategy to use for this scene type
     */
    virtual rg::Strategy* getRenderStrategy() override;

protected:
    /**
     * @brief Called when an object is added to the scene. Derived should create the SceneObject
     *        here and initialize descriptor sets
     *
     * @param entity The ECS entity of the new object
     * @param object The ECS component being added
     * @param sceneId The id of the new object in this scene
     * @param updateFreq Whether the object is static or dynamic
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, rcom::DrawableBase& object,
                                      UpdateSpeed updateFreq) override;

    /**
     * @brief Removes the object from the scene. Also removes all children
     *
     * @param object The object to be removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void doObjectRemoval(scene::SceneObject* object,
                                 mat::MaterialPipeline* pipeline) override;

    /**
     * @brief Called by Scene in handleDescriptorSync for objects that need to be re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change,
                               mat::MaterialPipeline* ogPipeline) override;

    /**
     * @brief Creates an overlay camera
     */
    virtual std::unique_ptr<cam::Camera> createDefaultCamera() override;

    /**
     * @brief Noop
     */
    virtual void setDefaultNearAndFarPlanes(cam::Camera&) const override {}

private:
    ecs::ComponentPool<ovy::OverlayObject>* ecsPool;
    scene::SceneObjectECSAdaptor<ovy::OverlayObject> objects;
    sys::OverlayScalerSystem& scaler;
    std::vector<ovy::OverlayObject*> roots;
    bool needRefreshAll;
    sig::Emitter<event::SceneObjectRemoved> emitter;

    std::vector<ovy::OverlayObject*> renderStack;
    VkViewport cachedParentViewport;
    glm::u32vec2 cachedTargetSize;

    void refreshAll();
    void sortRoots();

    virtual void process(const ecs::event::EntityParentSet& event) override;
    virtual void process(const ecs::event::EntityParentRemoved& event) override;
    virtual void process(const ecs::event::ComponentRemoved<ovy::OverlayObject>& event) override;

    template<typename T>
    friend class sys::DrawableSystem;
    friend class sys::OverlayScalerSystem;
    friend class Observer;
};

} // namespace rc
} // namespace bl

#endif
