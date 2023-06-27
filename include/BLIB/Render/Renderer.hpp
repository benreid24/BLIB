#ifndef BLIB_RENDER_RENDERER_HPP
#define BLIB_RENDER_RENDERER_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Observer.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Render/Resources/PipelineCache.hpp>
#include <BLIB/Render/Resources/PipelineLayoutCache.hpp>
#include <BLIB/Render/Resources/RenderPassCache.hpp>
#include <BLIB/Render/Resources/ScenePool.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Vulkan/RenderTexture.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Util/NonCopyable.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace gfx
{
namespace sys
{
class CameraUpdateSystem;
class RenderSystem;
} // namespace sys

/**
 * @brief Top level rendering system of the game engine
 *
 * @ingroup Renderer
 *
 */
class Renderer : private util::NonCopyable {
public:
    enum struct SplitscreenDirection { TopAndBottom, LeftAndRight };

    /**
     * @brief Adds an observer to the renderer. By default there is a single observer already. Call
     *        this for splitscreen
     *
     * @return A reference to the new observer
     */
    Observer& addObserver();

    /**
     * @brief Returns the observer at the given index
     *
     * @param i The index of the observer to get
     * @return The observer at the given index
     */
    Observer& getObserver(unsigned int i = 0);

    /**
     * @brief Removes the observer at the given index
     *
     * @param i The index of the observer to remove
     */
    void removeObserver(unsigned int i = 4);

    /**
     * @brief Returns the number of observers in the renderer, excluding the common observer
     */
    unsigned int observerCount() const;

    /**
     * @brief Allocates a single scene, pushes it to all observers, and returns it
     *
     * @tparam TScene The type of scene to create
     * @tparam TArgs Argument types to the scene's constructor
     * @param args Arguments to the scene's constructor
     * @return A pointer to the new scene active in all observers
     */
    template<typename TScene, typename... TArgs>
    TScene* pushSceneToAllObservers(TArgs&&... args);

    /**
     * @brief Removes the current scene from all observers
     */
    void popSceneFromAllObservers();

    /**
     * @brief Pops the current scene from all observers and returns it without releasing it from the
     *        scene pool. Note that only a single scene is returned, so care should be taken to only
     *        call this when all observers are using the same scene
     *
     * @return The scene that was removed
     */
    Scene* popSceneFromAllObserversNoRelease();

    /**
     * @brief Sets the default near and far values to be used for all cameras
     *
     * @param near The distance of the near plane
     * @param Far The distance of the far plane
     */
    void setDefaultNearAndFar(float near, float far);

    /**
     * @brief Returns the common observer for the renderer. Use this to render scenes that should be
     *        fullscreen regardless of the observer count (ie cutscenes). If the common observer has
     *        any active scenes then the other observers will not render their scenes
     *
     * @return A reference to the common observer
     */
    constexpr Observer& getCommonObserver();

    /**
     * @brief Sets the direction the screen will split if there is more than one observer. Default
     *        is SplitscreenDirection::TopAndBottom
     *
     * @param direction Whether to split horizontally or vertically
     */
    void setSplitscreenDirection(SplitscreenDirection direction);

    /**
     * @brief Returns the Vulkan state of the renderer
     */
    constexpr vk::VulkanState& vulkanState();

    /**
     * @brief Returns the texture pool of this renderer
     */
    constexpr res::TexturePool& texturePool();

    /**
     * @brief Returns the material pool of this renderer
     */
    constexpr res::MaterialPool& materialPool();

    /**
     * @brief Returns the render pass cache of this renderer
     */
    constexpr res::RenderPassCache& renderPassCache();

    /**
     * @brief Returns the pipeline cache of this renderer
     */
    constexpr res::PipelineCache& pipelineCache();

    /**
     * @brief Returns the pipeline layout cache of this renderer
     */
    constexpr res::PipelineLayoutCache& pipelineLayoutCache();

    /**
     * @brief Returns the scene pool of the renderer
     */
    constexpr res::ScenePool& scenePool();

    /**
     * @brief Returns a reference to the descriptor set factory cache used by this renderer
     */
    constexpr ds::DescriptorSetFactoryCache& descriptorFactoryCache();

    /**
     * @brief Sets the color to clear the window with prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const glm::vec3& color);

    /**
     * @brief Returns the distance of the default near plane for observers and cameras
     */
    constexpr float defaultNearPlane() const;

    /**
     * @brief Returns the distance of the default far plane for observers and cameras
     */
    constexpr float defaultFarPlane() const;

private:
    engine::Engine& engine;
    sf::WindowBase& window;
    sf::Rect<std::uint32_t> renderRegion;
    vk::VulkanState state;
    vk::PerSwapFrame<vk::Framebuffer> framebuffers;
    res::TexturePool textures;
    res::MaterialPool materials;
    ds::DescriptorSetFactoryCache descriptorSetFactoryCache;
    res::RenderPassCache renderPasses;
    res::PipelineLayoutCache pipelineLayouts;
    res::PipelineCache pipelines;
    res::ScenePool scenes;
    SplitscreenDirection splitscreenDirection;
    Observer commonObserver;
    std::vector<std::unique_ptr<Observer>> observers;
    float defaultNear, defaultFar;
    VkClearValue clearColors[2];
    std::vector<vk::RenderTexture*> renderTextures;

    Renderer(engine::Engine& engine, sf::WindowBase& window);
    ~Renderer();
    void initialize();
    void cleanup();
    void processResize(const sf::Rect<std::uint32_t>& region);

    void registerRenderTexture(vk::RenderTexture* rt);
    void removeRenderTexture(vk::RenderTexture* rt);

    // render stages
    void updateCameras(float dt);
    void renderFrame();

    void assignObserverRegions();

    friend class engine::Engine;
    friend class Observer;
    friend class sys::CameraUpdateSystem;
    friend class sys::RenderSystem;
    friend class vk::RenderTexture;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr vk::VulkanState& Renderer::vulkanState() { return state; }

inline constexpr res::TexturePool& Renderer::texturePool() { return textures; }

inline constexpr res::MaterialPool& Renderer::materialPool() { return materials; }

inline constexpr res::RenderPassCache& Renderer::renderPassCache() { return renderPasses; }

inline constexpr res::PipelineCache& Renderer::pipelineCache() { return pipelines; }

inline constexpr res::PipelineLayoutCache& Renderer::pipelineLayoutCache() {
    return pipelineLayouts;
}

inline constexpr res::ScenePool& Renderer::scenePool() { return scenes; }

inline constexpr ds::DescriptorSetFactoryCache& Renderer::descriptorFactoryCache() {
    return descriptorSetFactoryCache;
}

inline constexpr Observer& Renderer::getCommonObserver() { return commonObserver; }

inline Observer& Renderer::getObserver(unsigned int i) { return *observers[i]; }

template<typename TScene, typename... TArgs>
TScene* Renderer::pushSceneToAllObservers(TArgs&&... args) {
    TScene* s = scenes.allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    for (auto& o : observers) { o->pushScene(s); }
    return s;
}

template<typename TScene, typename... TArgs>
TScene* Observer::pushScene(TArgs&&... args) {
    TScene* s = renderer.scenePool().allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    scenes.emplace_back(renderer, s);
    onSceneAdd();
    return s;
}

template<typename TScene, typename... TArgs>
TScene* vk::RenderTexture::setScene(TArgs&&... args) {
    TScene* s = renderer.scenePool().allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    scene     = s;
    onSceneSet();
    return s;
}

inline constexpr float Renderer::defaultNearPlane() const { return defaultNear; }

inline constexpr float Renderer::defaultFarPlane() const { return defaultFar; }

} // namespace gfx
} // namespace bl

#endif
