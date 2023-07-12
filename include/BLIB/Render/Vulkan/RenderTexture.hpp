#ifndef BLIB_RENDER_RESOURCES_RENDERTEXTURE_HPP
#define BLIB_RENDER_RESOURCES_RENDERTEXTURE_HPP

#include <BLIB/Render/Cameras/Camera.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <BLIB/Util/NonCopyable.hpp>
#include <memory>

namespace bl
{
namespace rc
{
class Renderer;

namespace vk
{
/**
 * @brief Represents textures that can be rendered to and then sampled from. Render textures get
 *        rendered each frame prior to Observers rendering
 *
 * @ingroup Renderer
 */
class RenderTexture : private util::NonCopyable {
public:
    /**
     * @brief Empty initialization
     */
    RenderTexture();

    /**
     * @brief Destroys the textures if still created
     */
    ~RenderTexture();

    /**
     * @brief Creates (or re-creates) the textures and depth buffers
     *
     * @param renderer The renderer instance
     * @param size The size of the textures to create
     * @param sampler The sampler to use on the texture
     */
    void create(Renderer& renderer, const glm::u32vec2& size, VkSampler sampler = nullptr);

    /**
     * @brief Destroys the texture
     */
    void destroy();

    /**
     * @brief Returns the texture in the texture pool that are being rendered to
     */
    constexpr const res::TextureRef& getTexture() const;

    /**
     * @brief Sets the color to clear the texture to prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const glm::vec4& color);

    /**
     * @brief Returns whether or not a scene has been set
     */
    constexpr bool hasScene() const;

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
     * @brief Creates a new scene and sets it to be rendered
     *
     * @tparam TScene The type of scene to create
     * @tparam TArgs Argument types to the scene's constructor
     * @param args Arguments to the scene's constructor
     * @return The newly created, now active, scene
     */
    template<typename TScene, typename... TArgs>
    TScene* setScene(TArgs&&... args);

    /**
     * @brief Sets the current scene to render
     *
     * @param scene The scene to make active
     */
    void setScene(Scene* scene);

    /**
     * @brief Removes the current scene. Textures are still cleared each frame
     *
     * @ingroup clearCamera True to also free the camera, false to keep it
     */
    void removeScene(bool clearCamera = true);

    /**
     * @brief Returns the size of the textures in pixels
     */
    constexpr glm::u32vec2 getSize() const;

private:
    // TODO - consider separate storage in TexturePool for double-buffering
    // or allow Texture to double-buffer
    Renderer* renderer;
    res::TextureRef texture;
    AttachmentBuffer depthBuffer;
    StandardAttachmentSet attachmentSet;
    Framebuffer framebuffer;
    VkRect2D scissor;
    VkViewport viewport;
    VkClearValue clearColors[2];
    float defaultNear, defaultFar;

    Scene* scene;
    std::unique_ptr<Camera> camera;
    std::uint32_t observerIndex;

    void ensureCamera();
    void onSceneSet();

    // called by renderer
    void handleDescriptorSync();
    void updateCamera(float dt);
    void renderScene(VkCommandBuffer commandBuffer);

    friend class bl::rc::Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const res::TextureRef& RenderTexture::getTexture() const { return texture; }

inline constexpr bool RenderTexture::hasScene() const { return scene != nullptr; }

inline constexpr glm::u32vec2 RenderTexture::getSize() const {
    return {scissor.extent.width, scissor.extent.height};
}

template<typename TCamera, typename... TArgs>
TCamera* RenderTexture::setCamera(TArgs&&... args) {
    TCamera* cam = new TCamera(std::forward<TArgs>(args)...);
    static_cast<Camera*>(cam)->setNearAndFarPlanes(defaultNear, defaultFar);
    camera.reset(cam);
    return cam;
}

} // namespace vk
} // namespace rc
} // namespace bl

#endif
