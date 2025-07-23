#ifndef BLIB_RENDER_RENDERER_HPP
#define BLIB_RENDER_RENDERER_HPP

#include <BLIB/Render/Color.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetFactoryCache.hpp>
#include <BLIB/Render/Graph/AssetFactory.hpp>
#include <BLIB/Render/Graph/Strategy.hpp>
#include <BLIB/Render/Observer.hpp>
#include <BLIB/Render/Resources/GlobalDescriptors.hpp>
#include <BLIB/Render/Resources/MaterialPipelineCache.hpp>
#include <BLIB/Render/Resources/MaterialPool.hpp>
#include <BLIB/Render/Resources/PipelineCache.hpp>
#include <BLIB/Render/Resources/PipelineLayoutCache.hpp>
#include <BLIB/Render/Resources/RenderPassCache.hpp>
#include <BLIB/Render/Resources/SamplerCache.hpp>
#include <BLIB/Render/Resources/ScenePool.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Scenes/SceneSync.hpp>
#include <BLIB/Render/Settings.hpp>
#include <BLIB/Render/Transfers/TextureExporter.hpp>
#include <BLIB/Render/Vulkan/RenderTexture.hpp>
#include <BLIB/Render/Vulkan/VulkanState.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <future>

namespace bl
{
namespace engine
{
class Engine;
}
namespace sys
{
class RendererUpdateSystem;
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
     * @brief Returns the observer at the given index
     *
     * @param i The index of the observer to get
     * @return The observer at the given index
     */
    Observer& getObserver(unsigned int i = 0);

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
     * @brief Adds a new virtual observer to render to the given region. Virtual observers are
     *        separate from regular observers and may render to arbitrary screen regions. They
     *        render on top of the main observers
     *
     * @param region The screen scissor to render to. Viewport is set from this
     * @return A reference to the new observer
     */
    Observer& addVirtualObserver(const VkRect2D& region);

    /**
     * @brief Destroys the given virtual observer
     *
     * @param observer The observer to destroy
     */
    void destroyVirtualObserver(const Observer& observer);

    /**
     * @brief Create a render texture from the texture pool that can be rendered to
     *
     * @param size The size of the texture to create
     * @param sampler Optional sampler to use
     * @return A handle to the render texture
     */
    vk::RenderTexture::Handle createRenderTexture(
        const glm::u32vec2& size,
        vk::SamplerOptions::Type sampler = vk::SamplerOptions::Type::FilteredBorderClamped);

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
     * @brief Returns the sampler cache
     */
    res::SamplerCache& samplerCache();

    /**
     * @brief Returns the render pass cache of this renderer
     */
    res::RenderPassCache& renderPassCache();

    /**
     * @brief Returns the pipeline cache of this renderer
     */
    res::PipelineCache& pipelineCache();

    /**
     * @brief Returns the material pipeline cache of this renderer
     */
    res::MaterialPipelineCache& materialPipelineCache();

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
     * @brief Returns the texture exporter system
     */
    tfr::TextureExporter& textureExporter();

    /**
     * @brief Sets the color to clear the window with prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const Color& color);

    /**
     * @brief Returns the graph asset factory used by the renderer
     */
    rg::AssetFactory& getAssetFactory();

    /**
     * @brief Returns the global descriptor data instance
     */
    res::GlobalDescriptors& getGlobalDescriptorData();

    /**
     * @brief Returns the render settings which can be changed
     */
    Settings& getSettings();

    /**
     * @brief Returns the render settings
     */
    const Settings& getSettings() const;

private:
    std::mutex renderMutex;
    engine::Engine& engine;
    engine::EngineWindow& window;
    Settings settings;
    sf::Rect<std::uint32_t> renderRegion;
    vk::VulkanState state;
    res::GlobalDescriptors globalDescriptors;
    res::TexturePool textures;
    res::MaterialPool materials;
    res::SamplerCache samplers;
    ds::DescriptorSetFactoryCache descriptorSetFactoryCache;
    res::RenderPassCache renderPasses;
    res::PipelineLayoutCache pipelineLayouts;
    res::PipelineCache pipelines;
    res::MaterialPipelineCache materialPipelines;
    res::ScenePool scenes;
    tfr::TextureExporter imageExporter;
    SplitscreenDirection splitscreenDirection;
    Observer commonObserver;
    std::vector<std::unique_ptr<Observer>> observers;
    std::vector<std::unique_ptr<Observer>> virtualObservers;
    VkClearValue clearColors[2];
    std::vector<std::unique_ptr<vk::RenderTexture>> renderTextures;
    rg::AssetFactory assetFactory;
    scene::SceneSync sceneSync;

    Renderer(engine::Engine& engine, engine::EngineWindow& window);
    ~Renderer();
    void initialize();
    void cleanup();
    void processResize(const sf::Rect<std::uint32_t>& region);
    void processWindowRecreate();

    void destroyRenderTexture(vk::RenderTexture* rt);

    // render stages
    void update(float dt);
    void syncSceneObjects();
    void renderFrame();

    Observer& addObserver();
    void removeObserver(unsigned int i);
    void assignObserverRegions();

    friend class engine::Engine;
    friend class Observer;
    friend class sys::RendererUpdateSystem;
    friend class vk::RenderTexture::Handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline vk::VulkanState& Renderer::vulkanState() { return state; }

inline const vk::VulkanState& Renderer::vulkanState() const { return state; }

inline res::TexturePool& Renderer::texturePool() { return textures; }

inline res::MaterialPool& Renderer::materialPool() { return materials; }

inline res::SamplerCache& Renderer::samplerCache() { return samplers; }

inline res::RenderPassCache& Renderer::renderPassCache() { return renderPasses; }

inline res::PipelineCache& Renderer::pipelineCache() { return pipelines; }

inline res::MaterialPipelineCache& Renderer::materialPipelineCache() { return materialPipelines; }

inline res::PipelineLayoutCache& Renderer::pipelineLayoutCache() { return pipelineLayouts; }

inline res::ScenePool& Renderer::scenePool() { return scenes; }

inline ds::DescriptorSetFactoryCache& Renderer::descriptorFactoryCache() {
    return descriptorSetFactoryCache;
}

inline tfr::TextureExporter& Renderer::textureExporter() { return imageExporter; }

inline res::GlobalDescriptors& Renderer::getGlobalDescriptorData() { return globalDescriptors; }

inline Observer& Renderer::getCommonObserver() { return commonObserver; }

inline Observer& Renderer::getObserver(unsigned int i) { return *observers[i]; }

template<typename TScene, typename... TArgs>
SceneRef Renderer::pushSceneToAllObservers(TArgs&&... args) {
    SceneRef s = scenes.allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    for (auto& o : observers) { o->pushScene(s); }
    return s;
}

template<typename TScene, typename... TArgs>
SceneRef RenderTarget::pushScene(TArgs&&... args) {
    static_assert(std::is_base_of_v<Scene, TScene>, "Scene must derive from Scene");

    SceneRef s = renderer.scenePool().allocateScene<TScene, TArgs...>(std::forward<TArgs>(args)...);
    scenes.emplace_back(engine, renderer, this, graphAssets, s);
    onSceneAdd();
    return s;
}

inline rg::AssetFactory& Renderer::getAssetFactory() { return assetFactory; }

inline Settings& Renderer::getSettings() { return settings; }

inline const Settings& Renderer::getSettings() const { return settings; }

} // namespace rc
} // namespace bl

#endif
