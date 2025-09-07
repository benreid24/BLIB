#ifndef BLIB_RENDER_DESCRIPTORS_SHADERINPUT_HPP
#define BLIB_RENDER_DESCRIPTORS_SHADERINPUT_HPP

#include <BLIB/Render/Scenes/Key.hpp>

namespace bl
{
namespace engine
{
class Engine;
}

namespace rc
{
class Scene;

namespace vk
{
struct VulkanState;
}

namespace ds
{
/**
 * @brief Base class for shader inputs. Shader inputs are the images and buffers that descriptors
 *        point to. Descriptor sets share shader inputs for common data via the ShaderInputStore
 *
 * @ingroup Renderer
 */
class ShaderInput {
public:
    /**
     * @brief Destroys the shader input
     */
    virtual ~ShaderInput() = default;

    /**
     * @brief Derived classes should allocate resources here
     *
     * @param engine The game engine instance
     * @param vulkanState The renderer Vulkan state
     * @param owner The render target that owns the input
     * @param entityMapCb Callback to map scene keys to ECS entities
     */
    virtual void init(engine::Engine& engine, vk::VulkanState& vulkanState, Scene& owner,
                      const scene::MapKeyToEntityCb& entityMapCb) = 0;

    /**
     * @brief Derived classes should release resources here
     */
    virtual void cleanup() = 0;

    /**
     * @brief Derived classes should copy local buffers to the GPU here
     */
    virtual void performGpuSync() = 0;

    /**
     * @brief Derived classes should copy source data in to local buffers here
     */
    virtual void copyFromSource() = 0;

    /**
     * @brief Returns true if the dynamic descriptor sets need to be updated this frame
     */
    virtual bool dynamicDescriptorUpdateRequired() const = 0;

    /**
     * @brief Returns true if the static descriptor sets need to be updated this frame
     */
    virtual bool staticDescriptorUpdateRequired() const = 0;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
