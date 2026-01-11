#ifndef BLIB_RENDER_RESOURCES_RENDERTEXTURE_HPP
#define BLIB_RENDER_RESOURCES_RENDERTEXTURE_HPP

#include <BLIB/Render/RenderTarget.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Scenes/Scene.hpp>
#include <BLIB/Render/Vulkan/DedicatedCommandBuffers.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/SamplerOptions.hpp>
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
class RenderTexture : public RenderTarget {
public:
    /**
     * @brief Handle for render textures. Render textures themselves are owned by Renderer
     */
    class Handle {
    public:
        /**
         * @brief Creates an empty handle
         */
        Handle();

        /**
         * @brief Deleted
         */
        Handle(const Handle&) = delete;

        /**
         * @brief Moves from the given handle and assumes ownership of the texture
         *
         * @param c The handle to move from
         */
        Handle(Handle&& c);

        /**
         * @brief Releases the texture if any
         */
        ~Handle();

        /**
         * @brief Deleted
         */
        Handle& operator=(const Handle&) = delete;

        /**
         * @brief Moves from the given handle and assumes ownership of the texture
         *
         * @param c The handle to move from
         * @return A reference to this handle
         */
        Handle& operator=(Handle&& c);

        /**
         * @brief Returns whether the handle has a valid texture
         */
        bool isValid() const;

        /**
         * @brief Releases the texture if any
         */
        void release();

        /**
         * @brief Returns the underlying texture
         */
        RenderTexture* operator->();

        /**
         * @brief Returns the underlying texture
         */
        RenderTexture& operator*();

        /**
         * @brief Returns whether the handle has a valid texture
         */
        operator bool() const;

    private:
        Renderer* renderer;
        RenderTexture* texture;

        Handle(Renderer* r, RenderTexture* t);

        friend class ::bl::rc::Renderer;
    };

    /**
     * @brief Destroys the textures if still created
     */
    virtual ~RenderTexture();

    /**
     * @brief Resizes the texture being rendered to
     *
     * @param newSize The new size of the texture
     */
    void resize(const glm::u32vec2& newSize);

    /**
     * @brief Destroys the texture
     */
    void destroy();

    /**
     * @brief Returns the texture in the texture pool that are being rendered to
     */
    const res::TextureRef& getTexture() const;

    /**
     * @brief Returns the size of the textures in pixels
     */
    glm::u32vec2 getSize() const;

    using RenderTarget::clearScenes;
    using RenderTarget::popScene;
    using RenderTarget::popSceneNoRelease;
    using RenderTarget::pushScene;
    using RenderTarget::removeScene;

private:
    DedicatedCommandBuffers commandBuffers;
    res::TextureRef texture;

    // called by renderer
    RenderTexture(engine::Engine& engine, Renderer& renderer, rg::AssetFactory& factory,
                  const glm::u32vec2& size,
                  SamplerOptions::Type sampler = SamplerOptions::Type::FilteredBorderClamped);
    void render();

    friend class bl::rc::Renderer;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

inline const res::TextureRef& RenderTexture::getTexture() const { return texture; }

inline glm::u32vec2 RenderTexture::getSize() const {
    return {scissor.extent.width, scissor.extent.height};
}

inline RenderTexture::Handle::Handle()
: renderer(nullptr)
, texture(nullptr) {}

inline RenderTexture::Handle::Handle(Renderer* r, RenderTexture* t)
: renderer(r)
, texture(t) {}

inline RenderTexture::Handle::Handle(Handle&& c)
: renderer(c.renderer)
, texture(c.texture) {
    c.texture = nullptr;
}

inline RenderTexture::Handle::~Handle() { release(); }

inline RenderTexture::Handle& RenderTexture::Handle::operator=(Handle&& c) {
    release();
    renderer  = c.renderer;
    texture   = c.texture;
    c.texture = nullptr;
    return *this;
}

inline bool RenderTexture::Handle::isValid() const { return texture != nullptr; }

inline RenderTexture& RenderTexture::Handle::operator*() { return *texture; }

inline RenderTexture* RenderTexture::Handle::operator->() { return texture; }

inline RenderTexture::Handle::operator bool() const { return isValid(); }

} // namespace vk
} // namespace rc
} // namespace bl

#endif
