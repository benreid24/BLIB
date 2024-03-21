#ifndef BLIB_RENDER_RENDERER_HPP
#define BLIB_RENDER_RENDERER_HPP

#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Graph/AssetFactory.hpp>
#include <BLIB/Render/Graph/Strategy.hpp>
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
namespace sys
{
class RendererUpdateSystem;
class RenderSystem;
} // namespace sys

/// Renderer core implementation classes and functionality
namespace rc
{
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
    SceneRef pushSceneToAllObservers(TArgs&&... args);

    /**
     * @brief Removes the current scene from all observers
     */
    void popSceneFromAllObservers();

    /**
     * @brief Returns the common observer for the renderer. Use this to render scenes that should be
     *        fullscreen regardless of the observer count (ie cutscenes). If the common observer has
     *        any active scenes then the other observers will not render their scenes
     *
     * @return A reference to the common observer
     */
    Observer& getCommonObserver();

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
    vk::VulkanState& vulkanState();

    /**
     * @brief Returns the Vulkan state of the renderer
     */
    const vk::VulkanState& vulkanState() const;

    /**
     * @brief Returns the texture pool of this renderer
     */
    res::TexturePool& texturePool();

    /**
     * @brief Returns the material pool of this renderer
     */
    res::MaterialPool& materialPool();

    /**
     * @brief Returns the render pass cache of this renderer
     */
    res::RenderPassCache& renderPassCache();

    /**
     * @brief Returns the pipeline cache of this renderer
     */
    res::PipelineCache& pipelineCache();

    /**
     * @brief Returns the pipeline layout cache of this renderer
     */
    res::PipelineLayoutCache& pipelineLayoutCache();

    /**
     * @brief Returns the scene pool of the renderer
     */
    res::ScenePool& scenePool();

    /**
     * @brief Returns a reference to the descriptor set factory cache used by this renderer
     */
    ds::DescriptorSetFactoryCache& descriptorFactoryCache();

    /**
     * @brief Sets the color to clear the window with prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const glm::vec3& color);

    /**
     * @brief Replaces the current strategy with a new one of type T. Default is
     *        rgi::ForwardRendererStrategy
     *
     * @tparam T The new strategy type
     * @tparam ...TArgs Argument types to the strategy's constructor
     * @param ...args Arguments to the strategy's constructor
     * @return A pointer to the new strategy
     */
    template<typename T, typename... TArgs>
    T* useRenderStrategy(TArgs&&... args);

    /**
     * @brief Returns the current renderer strategy
     */
    rg::Strategy& getRenderStrategy();

    /**
     * @brief Returns the graph asset factory used by the renderer
     */
    rg::AssetFactory& getAssetFactory();

    /**
     * @brief Returns the framebuffers for the swap chain frames
     */
    vk::PerSwapFrame<vk::Framebuffer>& getSwapframeBuffers();

private:
    engine::Engine& engine;
    engine::EngineWindow& window;
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
    VkClearValue clearColors[2];
    std::vector<vk::RenderTexture*> renderTextures;
    std::unique_ptr<rg::Strategy> strategy;
    rg::AssetFactory assetFactory;

    Renderer(engine::Engine& engine, engine::EngineWindow& window);
    ~Renderer();
    void initialize();
    void cleanup();
    void processResize(const sf::Rect<std::uint32_t>& region);
    void processWindowRecreate();

    void registerRenderTexture(vk::RenderTexture* rt);
    void removeRenderTexture(vk::RenderTexture* rt);

    // render stages
    void update(float dt);
    void renderFrame();

    void assignObserverRegions();

    friend class engine::Engine;
    friend class Observer;
    friend class sys::RendererUpdateSystem;
    friend class sys::RenderSystem;
    friend class vk::RenderTexture;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline vk::VulkanState& Renderer::vulkanState() { return state; }

inline const vk::VulkanState& Renderer::vulkanState() const { return state; }

inline res::TexturePool& Renderer::texturePool() { return textures; }

inline res::MaterialPool& Renderer::materialPool() { return materials; }

inline res::RenderPassCache& Renderer::renderPassCache() { return renderPasses; }

inline res::PipelineCache& Renderer::pipelineCache() { return pipelines; }

inline res::PipelineLayoutCache& Renderer::pipelineLayoutCache() { return pipelineLayouts; }

inline res::ScenePool& Renderer::scenePool() { return scenes; }

inline ds::DescriptorSetFactoryCache& Renderer::descriptorFactoryCache() {
    return descriptorSetFactoryCache;
}

inline Observer& Renderer::getCommonObserver() { return commonObserver; }

inline Observer& Renderer::getObserver(unsigned int i) { return *observers[i]; }

template<typename TScene, typename... TArgs>
SceneRef Renderer::pushSceneToAllObservers(TArgs&&... args) {
    SceneRef s = scenes.allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    for (auto& o : observers) { o->pushScene(s); }
    return s;
}

template<typename TScene, typename... TArgs>
SceneRef Observer::pushScene(TArgs&&... args) {
    static_assert(std::is_base_of_v<Scene, TScene>, "Scene must derive from Scene");

    SceneRef s = renderer.scenePool().allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    scenes.emplace_back(engine, renderer, this, graphAssets, s);
    onSceneAdd();
    return s;
}

template<typename TScene, typename... TArgs>
SceneRef vk::RenderTexture::setScene(TArgs&&... args) {
    scene = renderer->scenePool().allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    onSceneSet();
    return scene;
}

template<typename T, typename... TArgs>
T* Renderer::useRenderStrategy(TArgs&&... args) {
    static_assert(std::is_base_of_v<rg::Strategy, T>, "Strategy must derive from rg::Strategy");

    T* s = new T(std::forward<TArgs>(args)...);
    strategy.reset(s);
    return s;
}

inline rg::AssetFactory& Renderer::getAssetFactory() { return assetFactory; }

inline vk::PerSwapFrame<vk::Framebuffer>& Renderer::getSwapframeBuffers() { return framebuffers; }

} // namespace rc
} // namespace bl

#endif
