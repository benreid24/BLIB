#ifndef BLIB_RENDER_COMPONENTS_TEXTURE_HPP
#define BLIB_RENDER_COMPONENTS_TEXTURE_HPP

#include <BLIB/Render/Components/DescriptorComponentBase.hpp>
#include <BLIB/Render/Resources/TextureRef.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Util/Visitor.hpp>
#include <array>
#include <cstdint>
#include <variant>

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
    : payload(texture) {}

    /**
     * @brief Creates a texture component with the given textures
     *
     * @param texture A reference to a TexturePool Texture
     */
    Texture(const vk::PerFrame<res::TextureRef>& textures)
    : payload(textures) {}

    /**
     * @brief Sets the texture that this component points to
     *
     * @param texture The texture to change to
     */
    void setTexture(const res::TextureRef& texture) {
        payload = texture;
        markDirty();
    }

    /**
     * @brief Sets the texture that this component points to
     *
     * @param texture The texture to change to
     */
    void setTexture(const vk::PerFrame<res::TextureRef>& texture) {
        payload = texture;
        markDirty();
    }

    /**
     * @brief Returns the associated texture for this component
     */
    const res::TextureRef& getTexture() const {
        return std::visit(util::Visitor{
                              [](const res::TextureRef& t) -> const res::TextureRef& { return t; },
                              [](const vk::PerFrame<res::TextureRef>& t) -> const res::TextureRef& {
                                  return t.current();
                              },
                          },
                          payload);
    }

    /**
     * @brief Syncs the texture id to a scene buffer
     *
     * @param sceneTextureId Texture id contained in a scene buffer
     */
    void refreshDescriptor(std::uint32_t& sceneTextureId) {
        sceneTextureId =
            std::visit(util::Visitor{
                           [](const res::TextureRef& t) -> std::uint32_t { return t.id(); },
                           [](const vk::PerFrame<res::TextureRef>& t) -> std::uint32_t {
                               return t.current().id();
                           },
                       },
                       payload);
    }

private:
    std::variant<res::TextureRef, vk::PerFrame<res::TextureRef>> payload;
};

} // namespace com
} // namespace gfx
} // namespace bl

#endif
