#ifndef BLIB_RENDER_OBSERVER_HPP
#define BLIB_RENDER_OBSERVER_HPP

#include <BLIB/Cameras/Camera.hpp>
#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Render/Graph/AssetPool.hpp>
#include <BLIB/Render/Graph/Assets/FinalSwapframeAsset.hpp>
#include <BLIB/Render/Graph/RenderGraph.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>
#include <BLIB/Render/Resources/SceneRef.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentBuffers.hpp>
#include <BLIB/Vulkan.hpp>
#include <SFML/Window.hpp>
#include <memory>
#include <mutex>
#include <stack>
#include <utility>
#include <vector>

namespace bl
{
namespace rc
{
class Renderer;

/**
 * @brief Top level class for observers in the renderer. Corresponds to Engine Actors. Each observer
 *        may have it's own scene stack and overlay, but the Renderer may also have a common
 *        scene and overlay for all observers. Observers may use the same scene amongst
 *        themselves and simply render it from different perspectives
 *
 * @ingroup Renderer
 */
class Observer {
public:
    /**
     * @brief Releases resources
     */
    ~Observer();

    /**
     * @brief Creates a new scene on top the Observer's scene stack and returns it
     *
     * @tparam TScene The type of scene to create
     * @tparam TArgs Argument types to the scene's constructor
     * @param args Arguments to the scene's constructor
     * @return The newly created, now active, scene
     */
    template<typename TScene, typename... TArgs>
    SceneRef pushScene(TArgs&&... args);

    /**
     * @brief Pushes an existing scene onto the Observer's scene stack
     *
     * @param scene The scene to make active
     */
    void pushScene(SceneRef scene);

    /**
     * @brief Creates a new Overlay for the current scene for this Observer. Replaces the existing
     *        Overlay if one was present
     *
     * @return A pointer to the new Overlay
     */
    Overlay* createSceneOverlay();

    /**
     * @brief Returns the current overlay, creating a new one if necessary
     *
     * @return A pointer to the new Overlay
     */
    Overlay* getOrCreateSceneOverlay();

    /**
     * @brief Returns the current Overlay. Will be nullptr if one has not been created
     */
    Overlay* getCurrentOverlay();

    /**
     * @brief Returns the current scene of the observer. May be nullptr
     */
    Scene* getCurrentScene();

    /**
     * @brief Removes the top scene from the observer's scene stack and returns it. Does not release
     *        the scene back into the scene pool
     *
     * @return The scene that was removed
     */
    SceneRef popSceneNoRelease();

    /**
     * @brief Removes and releases the current active scene to the scene pool
     */
    void popScene();

    /**
     * @brief Removes all scenes from the observer
     */
    void clearScenes();

    /**
     * @brief Returns the number of scenes in this observer's scene stack
     */
    std::size_t sceneCount() const;

    /**
     * @brief Returns whether or not this observer has an active scene
     */
    bool hasScene() const;

    /**
     * @brief Replaces the camera to render the current scene with
     *
     * @tparam TCamera The type of camera to install
     * @tparam ...TArgs Argument types to the camera's constructor
     * @param ...args Arguments to the camera's constructor
     * @return A pointer to the new camera
     */
    template<typename TCamera, typename... TArgs>
    TCamera* setCamera(TArgs&&... args);

    /**
     * @brief Returns the current camera. May be nullptr
     */
    cam::Camera* getCurrentCamera();

    /**
     * @brief Returns the current camera and performs a checked cast to the desired type
     *
     * @tparam TCamera The type of camera that is expected
     * @return A pointer to the current camera. May be nullptr
     */
    template<typename TCamera>
    TCamera* getCurrentCamera();

    /**
     * @brief Sets the color to clear the observer's render region to prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const glm::vec4& color);

    /**
     * @brief Returns a raw pointer to the clear values for the observer
     */
    constexpr const VkClearValue* getClearColors() const;

    /**
     * @brief Returns the render graph for the current scene. Can be used to add custom steps
     */
    rg::RenderGraph& getRenderGraph();

    /**
     * @brief Returns the size of the observers render region in pixels
     */
    constexpr glm::u32vec2 getRegionSize() const;

    /**
     * @brief Transforms the given coordinate from screen space to world space using the current
     *        viewport and camera
     *
     * @param screenPoint The screen space coordinate to transform
     * @return The corresponding world space coordinate
     */
    glm::vec2 transformToWorldSpace(const glm::vec2& screenPoint) const;

    /**
     * @brief Transforms the given coordinate from screen space to overlay space using the current
     *        viewport and camera
     *
     * @param screenPoint The screen space coordinate to transform
     * @return The corresponding overlay space coordinate
     */
    glm::vec2 transformToOverlaySpace(const glm::vec2& screenPoint) const;

private:
    struct SceneInstance {
        SceneRef scene;
        SceneRef overlayRef;
        Overlay* overlay;
        rg::RenderGraph graph;
        std::uint32_t observerIndex;
        std::uint32_t overlayIndex;
        std::unique_ptr<cam::Camera> camera;

        SceneInstance(engine::Engine& e, Renderer& r, Observer* owner, rg::AssetPool& pool,
                      SceneRef s)
        : scene(s)
        , overlay(nullptr)
        , graph(e, r, pool, owner)
        , observerIndex(0)
        , overlayIndex(0) {}
    };

    const bool isCommon;
    engine::Engine& engine;
    Renderer& renderer;
    rg::AssetPool graphAssets;
    bool resourcesFreed;
    VkViewport viewport;
    VkRect2D scissor;
    rgi::FinalSwapframeAsset* swapframeAsset;
    std::vector<SceneInstance> scenes;
    VkClearValue clearColors[2];
    cam::OverlayCamera overlayCamera;
    glm::mat4 overlayProjView;

    Observer(engine::Engine& engine, Renderer& renderer, rg::AssetFactory& factory, bool isCommon);
    void handleDescriptorSync();
    void update(float dt);
    void assignRegion(const sf::Vector2u& windowSize, const sf::Rect<std::uint32_t>& parentRegion,
                      unsigned int observerCount, unsigned int index, bool topBottomFirst);
    void cleanup();
    void onSceneAdd();
    void onSceneChange();

    // called by Renderer
    void renderScene(VkCommandBuffer commandBuffer);
    void compositeSceneAndOverlay(VkCommandBuffer commandBuffer);

    friend class bl::rc::Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline std::size_t Observer::sceneCount() const { return scenes.size(); }

inline bool Observer::hasScene() const { return !scenes.empty(); }

inline Scene* Observer::getCurrentScene() {
    return !scenes.empty() ? scenes.back().scene.get() : nullptr;
}

template<typename TCamera, typename... TArgs>
TCamera* Observer::setCamera(TArgs&&... args) {
    if (hasScene()) {
        TCamera* cam = new TCamera(std::forward<TArgs>(args)...);
        scenes.back().scene->setDefaultNearAndFarPlanes(*cam);
        scenes.back().camera.reset(cam);
        return cam;
    }

    BL_LOG_ERROR << "Tried to set camera for observer with no current scene";
    return nullptr;
}

template<typename TCamera>
TCamera* Observer::getCurrentCamera() {
    if (hasScene()) {
        TCamera* camera = dynamic_cast<TCamera*>(scenes.back().camera.get());
        if (!camera) {
            BL_LOG_WARN << "Could not convert current camera to type: " << typeid(TCamera).name();
        }
        return camera;
    }

    BL_LOG_WARN << "Tried to retrieve current camera without a scene";
    return nullptr;
}

inline cam::Camera* Observer::getCurrentCamera() {
    if (hasScene()) { return scenes.back().camera.get(); }

    BL_LOG_WARN << "Tried to retrieve current camera without a scene";
    return nullptr;
}

inline rg::RenderGraph& Observer::getRenderGraph() { return scenes.back().graph; }

inline constexpr const VkClearValue* Observer::getClearColors() const { return clearColors; }

inline constexpr glm::u32vec2 Observer::getRegionSize() const {
    return {scissor.extent.width, scissor.extent.height};
}

} // namespace rc
} // namespace bl

#endif
