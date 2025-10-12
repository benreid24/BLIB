#ifndef BLIB_RENDER_DESCRIPTORS_SHADOWMAPBINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_SHADOWMAPBINDING_HPP

#include <BLIB/Render/Buffers/UniformBuffer.hpp>
#include <BLIB/Render/Descriptors/Builtin/CommonShaderInputs.hpp>
#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <BLIB/Render/Lighting/Scene3DLighting.hpp>
#include <array>
#include <glm/glm.hpp>

namespace bl
{
namespace rc
{
namespace dsi
{
class Scene3DInstance;

/**
 * @brief Binding for light camera matrices for rendering shadow maps
 *
 * @ingroup Renderer
 */
class ShadowMapBinding : public ds::Binding {
public:
    /// No externally accessible payload for this binding
    using TPayload = void;

    /**
     * @brief Creates the binding
     */
    ShadowMapBinding();

private:
    ShadowMapCameraShaderInput* storage;

    virtual ds::DescriptorSetInstance::EntityBindMode getBindMode() const override {
        return ds::DescriptorSetInstance::EntityBindMode::Bindless;
    }
    virtual ds::DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override {
        return ds::DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
    }

    virtual void init(vk::VulkanState& vulkanState, sr::ShaderResourceStore& globalShaderResources,
                      sr::ShaderResourceStore& sceneShaderResources,
                      sr::ShaderResourceStore& observerShaderResources) override;
    virtual void writeSet(ds::SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
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
    friend class ds::Bindings;
};

} // namespace dsi
} // namespace rc
} // namespace bl

#endif
