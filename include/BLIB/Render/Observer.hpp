#ifndef BLIB_RENDER_OBSERVER_HPP
#define BLIB_RENDER_OBSERVER_HPP

#include <BLIB/Render/Cameras/Camera.hpp>
#include <BLIB/Render/Overlays/Drawables/Image.hpp>
#include <BLIB/Render/Scenes/PostFX.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Util/PerFrame.hpp>
#include <BLIB/Render/Vulkan/StandardImageBuffer.hpp>
#include <SFML/Window.hpp>
#include <glad/vulkan.h>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>

namespace bl
{
namespace render
{
class Renderer;

/**
 * @brief Top level class for observers in the renderer. Corresponds to Engine Actors. Each observer
 *        may have it's own scene stack and 2d overlay, but the Renderer may also have a common
 *        scene and 2d overlay for all observers. Observers may use the same scene amongst
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
     * @param maxStaticObjectCount The maximum number of static objects in the scene
     * @param maxStaticObjectCount The maximum number of dynamic objects in the scene
     * @return The newly created, now active, scene
     */
    Scene* pushScene(std::uint32_t maxStaticObjectCount, std::uint32_t maxDynamicObjectCount);

    /**
     * @brief Pushes an existing scene onto the Observer's scene stack
     *
     * @param scene The scene to make active
     */
    void pushScene(Scene* scene);

    /**
     * @brief Removes the top scene from the observer's scene stack and returns it. Does not release
     *        the scene back into the scene pool
     *
     * @param popCamera True to also pop the top-most camera, false to leave it
     * @return The scene that was removed
     */
    Scene* popSceneNoRelease(bool popCamera = true);

    /**
     * @brief Removes and releases the current active scene to the scene pool
     *
     * @param popCamera True to also pop the top-most camera, false to leave it
     */
    void popScene(bool popCamera = true);

    /**
     * @brief Removes all scenes from the observer and returns them to the scene pool. Also clears
     *       all cameras. This should be called for observers being destructed if they own their own
     *       scenes. Care must be taken not to release scenes held by other observers
     */
    void clearScenes();

    /**
     * @brief Removes all scenes but does not return them to the scene pool. Also clears all cameras
     */
    void clearScenesNoRelease();

    /**
     * @brief Returns the number of scenes in this observer's scene stack
     */
    std::size_t sceneCount() const;

    /**
     * @brief Returns whether or not this observer has an active scene
     */
    bool hasScene() const;

    /**
     * @brief Installs the given camera to render scenes with
     *
     * @tparam TCamera The type of camera to install
     * @tparam ...TArgs Argument types to the camera's constructor
     * @param ...args Arguments to the camera's constructor
     * @return
     */
    template<typename TCamera, typename... TArgs>
    TCamera* pushCamera(TArgs&&... args);
    /**
     * @brief Replaces the current camera to render scenes with
     *
     * @tparam TCamera The type of camera to install
     * @tparam ...TArgs Argument types to the camera's constructor
     * @param ...args Arguments to the camera's constructor
     * @return
     */
    template<typename TCamera, typename... TArgs>
    TCamera* replaceCamera(TArgs&&... args);

    /**
     * @brief Removes the top-most camera
     */
    void popCamera();

    /**
     * @brief Removes all cameras
     */
    void clearCameras();

    /**
     * @brief Sets the post processing to be used when compositing the rendered scene. PostFX are
     *        only applied to the current scene. New scenes start with plain PostFX. Popping a scene
     *        will result in the prior scene using the same postFX it was using before
     *
     * @tparam FX Type of post processing to create
     * @tparam ...TArgs Argument types to create the new object
     * @param ...args Arguments to the constructor of the postfx object
     * @return The new post processing object
     */
    template<typename FX, typename... TArgs>
    FX* setPostFX(TArgs&&... args);

    /**
     * @brief Reverts the current scene's post processing to a standard copy
     */
    void removePostFX();

    /**
     * @brief Called once prior to the TransferEngine kicking off
     */
    void handleDescriptorSync();

    /**
     * @brief Records commands to render the observer's active scene to its internal image
     *
     * @param commandBuffer Command buffer to record into
     */
    void renderScene(VkCommandBuffer commandBuffer);

    /**
     * @brief Inserts pipeline barriers as required to ensure scene rendering is complete before the
     *        results are composited
     *
     * @param commandBuffer Command buffer to record into
     */
    void insertSceneBarriers(VkCommandBuffer commandBuffer);

    /**
     * @brief Records commands to render the rendered scene onto the given swapchain image, applying
     *        postfx if any are active. Must be called with an active render pass
     *
     * @param commandBuffer Command buffer to record into
     */
    void compositeSceneWithEffects(VkCommandBuffer commandBuffer);

    // TODO - overlay render method

private:
    Renderer& renderer;
    bool resourcesFreed;
    std::mutex mutex;
    PerFrame<StandardImageBuffer> renderFrames;
    PerFrame<Framebuffer> sceneFramebuffers;
    VkRect2D scissor;    // refreshed on window resize and observer add/remove
    VkViewport viewport; // derived from scissor. depth should be set by caller
    std::stack<Scene*, std::vector<Scene*>> scenes;
    std::stack<std::unique_ptr<Camera>, std::vector<std::unique_ptr<Camera>>> cameras;
    std::stack<std::unique_ptr<PostFX>, std::vector<std::unique_ptr<PostFX>>> postFX;
    float defaultNear, defaultFar;
    // TODO - 2d overlay for observer

    Observer(Renderer& renderer);
    void update(float dt);
    void assignRegion(const sf::WindowBase& window, unsigned int observerCount, unsigned int index,
                      bool topBottomFirst);
    void setDefaultNearFar(float near, float far);
    void cleanup();

    void onSceneAdd();
    void onScenePop();

    template<typename TCamera, typename... TArgs>
    TCamera* constructCamera(TArgs&&... args);

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline std::size_t Observer::sceneCount() const { return scenes.size(); }

inline bool Observer::hasScene() const { return !scenes.empty(); }

template<typename TCamera, typename... TArgs>
TCamera* Observer::constructCamera(TArgs&&... args) {
    TCamera* cam = new TCamera(std::forward<TArgs>(args)...);
    static_cast<Camera*>(cam)->setNearAndFarPlanes(defaultNear, defaultFar);
    return cam;
}

template<typename TCamera, typename... TArgs>
TCamera* Observer::pushCamera(TArgs&&... args) {
    std::unique_lock lock(mutex);

    TCamera* cam = constructCamera<TCamera>(std::forward<TArgs>(args)...);
    cameras.emplace(cam);
    return cam;
}

template<typename TCamera, typename... TArgs>
TCamera* Observer::replaceCamera(TArgs&&... args) {
    std::unique_lock lock(mutex);

    TCamera* cam = constructCamera<TCamera>(std::forward<TArgs>(args)...);
    if (!cameras.empty()) { cameras.top().reset(cam); }
    else { cameras.emplace(cam); }
    return cam;
}

template<typename FX, typename... TArgs>
FX* Observer::setPostFX(TArgs&&... args) {
    FX* fx = new FX(std::forward<TArgs>(args)...);
    postFX.top().reset(fx);
    return fx;
}

} // namespace render
} // namespace bl

#endif
