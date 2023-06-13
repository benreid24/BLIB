#ifndef BLIB_RENDER_OVERLAYS_OVERLAY_HPP
#define BLIB_RENDER_OVERLAYS_OVERLAY_HPP

#include <BLIB/ECS/Entity.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstanceCache.hpp>
#include <BLIB/Render/Descriptors/SceneDescriptorSetInstance.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
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

namespace render
{
class Renderer;

/**
 * @brief Special type of scene specialized for 2d overlays with fixed cameras. Objects are
 *        organized into a tree and rendered in tree order
 *
 * @ingroup Renderer
 */
class Overlay : public Scene {
public:
    static constexpr std::uint32_t NoParent    = std::numeric_limits<std::uint32_t>::max();
    static constexpr std::uint32_t PopViewport = NoParent;

    /**
     * @brief Creates a new overlay scene
     *
     * @param renderer The renderer instance
     * @param engine The game engine instance
     * @param maxStatic The maximum number of static objects
     * @param maxDynamic The maximum number of dynamic objects
     */
    Overlay(Renderer& renderer, engine::Engine& engine, std::uint32_t maxStatic,
            std::uint32_t maxDynamic);

    /**
     * @brief Frees resources
     */
    virtual ~Overlay() = default;

protected:
    /**
     * @brief Derived classes should record render commands in here
     *
     * @param context Render context containing scene render data
     */
    virtual void renderScene(scene::SceneRenderContext& context) override;

    /**
     * @brief Called when an object is added to the scene. Derived should create the SceneObject
     *        here and initialize descriptor sets
     *
     * @param entity The ECS entity of the new object
     * @param sceneId The id of the new object in this scene
     * @param updateFreq Whether the object is static or dynamic
     * @param pipelines Which pipelines to use to render the object
     * @return A pointer to the new scene object
     */
    virtual scene::SceneObject* doAdd(ecs::Entity entity, std::uint32_t sceneId,
                                      UpdateSpeed updateFreq,
                                      const scene::StagePipelines& pipelines) override;

    /**
     * @brief Called when an object is removed from the scene. Unlink from descriptors here
     *
     * @param entity The ECS entity being removed
     * @param object The object being removed
     * @param pipelines The pipelines used to render the object being removed
     */
    virtual void doRemove(ecs::Entity entity, scene::SceneObject* object,
                          const scene::StagePipelines& pipelines) override;

    /**
     * @brief Sets the parent object of the given child. Must be called after object add for an
     *        object to be rendered. Only call once per object
     *
     * @param child The object to set the parent of
     * @param parent The parent object, or NoParent to make root
     */
    void setParent(std::uint32_t child, std::uint32_t parent = NoParent);

private:
    struct ViewportPair {
        ViewportPair(const VkViewport& viewport);
        void apply(VkCommandBuffer commandBuffer);

        VkViewport viewport;
        VkRect2D scissor;
    };

    engine::Engine& engine;
    std::vector<ovy::OverlayObject> objects;
    std::vector<std::uint32_t> roots;
    std::vector<std::uint32_t> parentMap;

    std::vector<std::uint32_t> renderStack;
    std::vector<ViewportPair> viewportStack;

    template<typename T>
    friend class sys::GenericDrawableSystem;
    friend class Observer;
    friend class res::ScenePool;
};

} // namespace render
} // namespace bl

#endif