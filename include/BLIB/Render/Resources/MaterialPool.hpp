#ifndef BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP
#define BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP

#include <BLIB/Containers/RefPoolDirect.hpp>
#include <BLIB/Render/Materials/Material.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace res
{
/**
 * @brief Reference counted handle to a material
 *
 * @ingroup Renderer
 */
using MaterialRef = ctr::RefDirect<mat::Material>;

/**
 * @brief Collection of materials currently in use. Similar to TexturePool
 *
 * @ingroup Renderer
 */
class MaterialPool {
public:
    /**
     * @brief Creates or returns an existing material from a texture
     *
     * @param texture The texture the material should use
     * @return A ref to the material
     */
    MaterialRef getOrCreateFromTexture(const res::TextureRef& texture);

private:
    Renderer& renderer;
    ctr::RefPoolDirect<mat::Material> materials;

    MaterialPool(Renderer& renderer);

    friend class ::bl::rc::Renderer;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
