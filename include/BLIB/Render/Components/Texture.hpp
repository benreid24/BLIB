#ifndef BLIB_RENDER_COMPONENTS_TEXTURE_HPP
#define BLIB_RENDER_COMPONENTS_TEXTURE_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <cstdint>

namespace bl
{
namespace render
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
    Texture()
    : textureId(0) {}

    /**
     * @brief Creates a texture component with the given texture id
     *
     * @param textureId Texture id from a TexturePool
     */
    Texture(std::uint32_t textureId)
    : textureId(textureId) {}

    /**
     * @brief Modifies the texture of the object this component belongs to
     *
     * @param tid The texture id from a TexturePool
     */
    void setTextureId(std::uint32_t tid) {
        textureId = tid;
        markDirty();
    }

    /**
     * @brief Syncs the texture id to a scene buffer
     *
     * @param sceneTextureId Texture id contained in a scene buffer
     */
    void refreshDescriptor(std::uint32_t& sceneTextureId) { sceneTextureId = textureId; }

private:
    std::uint32_t textureId;
};

} // namespace com
} // namespace render
} // namespace bl

#endif
