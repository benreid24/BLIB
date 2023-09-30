#ifndef BLIB_RENDER_OVERLAYS_OVERLAY_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAY_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Scenes/SceneObjectECSAdaptor.hpp>
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
class Overlay : public Scene {
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
     * @brief Derived classes should record render commands in here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderScene(scene::SceneRenderContext& context) override;

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
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param entity The ECS entity being removed
     * @param object The object being removed
     * @param pipeline The pipeline used to render the object being removed
     */
    virtual void doRemove(scene::SceneObject* object, std::uint32_t pipeline) override;

    /**
     * @brief Called by Scene in handleDescriptorSync for objects that need to be re-batched
     *
     * @param change Details of the change
     * @param ogPipeline The original pipeline of the object being changed
     */
    virtual void doBatchChange(const BatchChange& change, std::uint32_t ogPipeline) override;

    /**
     * @brief Sets the parent object of the given child. Must be called after object add for an
     *        object to be rendered. Only call once per object
     *
     * @param child The object to set the parent of
     * @param parent The parent object, or NoParent to make root
     */
    void setParent(scene::Key child, ecs::Entity parent = ecs::InvalidEntity);

private:
    engine::Engine& engine;
    ecs::ComponentPool<ovy::OverlayObject>* ecsPool;
    scene::SceneObjectECSAdaptor<ovy::OverlayObject> objects;
    sys::OverlayScalerSystem& scaler;
    std::vector<ovy::OverlayObject*> roots; // TODO - use event to keep up to date
    std::vector<std::pair<scene::Key, ecs::Entity>> toParent;

    std::vector<ovy::OverlayObject*> renderStack;
    VkViewport cachedParentViewport;
    glm::u32vec2 cachedTargetSize;

    void applyParent(scene::Key child, ecs::Entity parent);
    void refreshAll();

    template<typename T>
    friend class sys::DrawableSystem;
    friend class sys::OverlayScalerSystem;
    friend class Observer;
};

} // namespace rc
} // namespace bl

#endif
