#ifndef BLIB_RENDER_COMPONENTS_TEXTURE_HPP
#define BLIB_RENDER_COMPONENTS_TEXTURE_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <cstdint>

namespace bl
{
namespace gfx
{
namespace com
{
/**
 * @brief ECS component to add a texture to an object
 *
 * @ingroup Renderer
 */
struct Texture : public DescriptorComponentBase<Texture, std::uint32_t> {
    /**
     * @brief Creates a texture component with no associated texture
     */
    Texture() = default;

    /**
     * @brief Creates a texture component with the given texture
     *
     * @param texture A reference to a TexturePool Texture
     */
    Texture(const res::TextureRef& texture)
    : texture(texture) {}

    /**
     * @brief Sets the texture that this component points to
     *
     * @param texture The texture to change to
     */
    void setTexture(const res::TextureRef& texture) {
        this->texture = texture;
        markDirty();
    }

    /**
     * @brief Returns the associated texture for this component
     */
    const res::TextureRef& getTexture() const { return texture; }

    /**
     * @brief Syncs the texture id to a scene buffer
     *
     * @param sceneTextureId Texture id contained in a scene buffer
     */
    void refreshDescriptor(std::uint32_t& sceneTextureId) { sceneTextureId = texture.id(); }

private:
    res::TextureRef texture;
};

} // namespace com
} // namespace gfx
} // namespace bl

#endif
