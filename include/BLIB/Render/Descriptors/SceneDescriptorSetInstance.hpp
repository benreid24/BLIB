#ifndef BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP
#define BLIB_RENDER_DESCRIPTORS_SCENEDESCRIPTORSETINSTANCE_HPP

#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Descriptors/SetWriteHelper.hpp>
#include <BLIB/Render/Vulkan/PerFrame.hpp>
#include <BLIB/Render/Vulkan/PerFrameVector.hpp>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
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
     * @brief Basic uniform struct containing observer info used during render
     */
    struct ObserverInfo {
        glm::mat4 projection;
        glm::mat4 view;
        alignas(16) glm::vec3 pos;

        ObserverInfo() = default;
        ObserverInfo(const glm::mat4& proj, const glm::mat4& view, const glm::vec3& pos)
        : projection(proj)
        , view(view)
        , pos(pos) {}
    };

    /**
     * @brief Creates the descriptor set instance
     *
     * @param vulkanState Renderer Vulkan state
     * @param setLayout The descriptor set layout
     */
    SceneDescriptorSetInstance(vk::VulkanState& vulkanState, VkDescriptorSetLayout setLayout);

    /**
     * @brief Destroys the descriptor set
     */
    virtual ~SceneDescriptorSetInstance() = default;

    /**
     * @brief Updates the camera value for the given observer
     *
     * @param observerIndex Index of the observer to update
     * @param info The observer's camera info
     */
    void updateObserverCamera(std::uint32_t observerIndex, const ObserverInfo& info);

    /**
     * @brief Returns the binding info to create set layouts with
     *
     * @return The descriptor binding info to use when creating set layouts
     * @param binding The binding index of the camera buffer
     * @param shaderStages The shader stages the camera buffer will be used in
     */
    static VkDescriptorSetLayoutBinding getCameraBufferBindingInfo(std::uint32_t binding,
                                                                   VkShaderStageFlags shaderStages);

protected:
    vk::VulkanState& vulkanState;
    const VkDescriptorSetLayout setLayout;
    vk::DescriptorPool::AllocationHandle allocHandle;
    vk::PerFrameVector<VkDescriptorSet> descriptorSets;
    buf::UniformBuffer<ObserverInfo> cameraBuffer;

    /**
     * @brief Allocates the descriptor set handles
     */
    void allocateDescriptorSets();

    /**
     * @brief Creates the camera buffer and sets up the transfer cadence
     */
    void createCameraBuffer();

    /**
     * @brief Issues write commands to the given set write helper for the camera buffer
     *
     * @param writer The writer to write info into
     * @param frameIndex The frame index of the buffer to bind
     * @param observerIndex The index of the observer to bind
     * @param binding The bind index to write at
     */
    void writeCameraDescriptor(SetWriteHelper& writer, std::uint32_t frameIndex,
                               std::uint32_t observerIndex, std::uint32_t binding = 0);

    /**
     * @brief Returns the dynamic binding offset for the given observer index
     *
     * @param observerIndex The index of the observer camera to get the offset of
     * @return The dynamic binding offset to use
     */
    std::uint32_t getBindDynamicOffset(std::uint32_t observerIndex) const;

    /**
     * @brief Destroys the camera buffer and releases the descriptor sets
     */
    void cleanup();
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
