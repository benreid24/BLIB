#ifndef BLIB_RENDER_RESOURCES_RENDERTEXTURE_HPP
#define BLIB_RENDER_RESOURCES_RENDERTEXTURE_HPP

#include <BLIB/Render/Cameras/Camera.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Vulkan/AttachmentBuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/StandardAttachmentSet.hpp>
#include <memory>

namespace bl
{
namespace gfx
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
class RenderTexture {
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
     * @brief Returns the textures in the texture pool that are being rendered to
     */
    constexpr const vk::PerFrame<res::TextureRef>& getTextures() const;

    /**
     * @brief Sets the color to clear the texture to prior to rendering
     *
     * @param color The color to clear with
     */
    void setClearColor(const glm::vec3& color);

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
     */
    void removeScene();

    /**
     * @brief Returns the size of the textures in pixels
     */
    constexpr glm::u32vec2 getSize() const;

private:
    Renderer* renderer;
    vk::PerFrame<res::TextureRef> textures;
    vk::PerFrame<vk::AttachmentBuffer> depthBuffers;
    vk::PerFrame<vk::StandardAttachmentSet> attachmentSets;
    vk::PerFrame<vk::Framebuffer> framebuffers;
    VkRect2D scissor;
    VkViewport viewport;
    VkClearValue clearColors[2];

    Scene* scene;
    std::unique_ptr<Camera> camera;
    std::uint32_t observerIndex;

    void handleDescriptorSync();
    void updateCamera(float dt);
    void renderScene(VkCommandBuffer commandBuffer);
    void ensureCamera();

    friend class Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline constexpr const vk::PerFrame<res::TextureRef>& RenderTexture::getTextures() const {
    return textures;
}

inline constexpr bool RenderTexture::hasScene() const { return scene != nullptr; }

inline constexpr glm::u32vec2 RenderTexture::getSize() const {
    return {scissor.extent.width, scissor.extent.height};
}

} // namespace vk
} // namespace gfx
} // namespace bl

#endif
