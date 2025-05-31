#ifndef BLIB_RENDER_DESCRIPTORS_SHADOWMAPBINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_SHADOWMAPBINDING_HPP

#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Config.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <BLIB/Render/Lighting/Scene3DLighting.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
class Scene3DInstance;

/**
 * @brief Binding for light camera matrices for rendering shadow maps
 *
 * @ingroup Renderer
 */
class ShadowMapBinding : public Binding {
public:
    /// No individual payload for this binding
    using TPayload = void;

    /**
     * @brief Creates the binding
     */
    ShadowMapBinding();

    /**
     * @brief Sets the lighting descriptor set to use for this binding
     *
     * @param l The scene lighting
     */
    void setLighting(Scene3DInstance* l);

private:
    using Payload = std::array<glm::mat4, 6>;

    Scene3DInstance* lighting;

    virtual DescriptorSetInstance::EntityBindMode getBindMode() const override {
        return DescriptorSetInstance::EntityBindMode::Bindless;
    }
    virtual DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override {
        return DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
    }

    virtual void init(vk::VulkanState& vulkanState, DescriptorComponentStorageCache&) override;
    virtual void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
                          std::uint32_t frameIndex) override;
    virtual bool allocateObject(ecs::Entity, scene::Key) override { return true; }
    virtual void releaseObject(ecs::Entity, scene::Key) override {}
    virtual void onFrameStart() override;
    virtual void* getPayload() override { return nullptr; }
    virtual bool staticDescriptorUpdateRequired() const override { return false; }
    virtual bool dynamicDescriptorUpdateRequired() const override { return false; }
    virtual std::uint32_t getDynamicOffsetForPipeline(scene::SceneRenderContext& ctx,
                                                      VkPipelineLayout, std::uint32_t,
                                                      UpdateSpeed) override;

    template<typename... TBindings>
    friend class Bindings;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif
