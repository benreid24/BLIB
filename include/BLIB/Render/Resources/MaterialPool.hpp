#ifndef BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP
#define BLIB_RENDER_RESOURCES_MATERIALPOOL_HPP

#include <BLIB/Containers/RefPoolDirect.hpp>
#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Materials/Material.hpp>
#include <BLIB/Render/Materials/MaterialDescriptor.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Vulkan.hpp>

namespace bl
{
namespace rc
{
class Renderer;

namespace res
{
class GlobalDescriptors;

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
    buf::UniformBuffer<mat::MaterialDescriptor> gpuPool;

    MaterialPool(Renderer& renderer);
    void init();
    void cleanup();
    void onFrameStart();

    friend class ::bl::rc::Renderer;
    friend class GlobalDescriptors;
};

} // namespace res
} // namespace rc
} // namespace bl

#endif
