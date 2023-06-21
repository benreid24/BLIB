#ifndef BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Transfers/UniformBuffer.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace gfx
{
namespace ds
{
/**
 * @brief Base class for descriptor sets for scene data (cameras, lighting, etc)
 *
 * @ingroup Renderer
 */
class SceneDescriptorSetInstance : public DescriptorSetInstance {
public:
    /**
     * @brief Creates the descriptor set instance
     */
    SceneDescriptorSetInstance();

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~SceneDescriptorSetInstance() = default;

    /**
     * @brief Updates the camera value for the given observer
     *
     * @param observerIndex Index of the observer to update
     * @param projView Camera matrix for the given observer
     */
    void updateObserverCamera(std::uint32_t observerIndex, const glm::mat4& projView);

protected:
    tfr::UniformBuffer<glm::mat4> cameraBuffer;
};

} // namespace ds
} // namespace gfx
} // namespace bl

#endif
