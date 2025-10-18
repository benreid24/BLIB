#ifndef BLIB_RENDER_DESCRIPTORS_GENERIC_ASSETBUFFERBINDING_HPP
#define BLIB_RENDER_DESCRIPTORS_GENERIC_ASSETBUFFERBINDING_HPP

#include <BLIB/Render/Descriptors/Generic/Binding.hpp>
#include <BLIB/Render/Graph/Assets/GenericBufferAsset.hpp>
#include <BLIB/Render/ShaderResources/AssetBufferShaderResource.hpp>

namespace bl
{
namespace rc
{
namespace ds
{
/**
 * @brief Binding for generic buffer bindings provided by buffers that live in the render graph
 *
 * @tparam TAsset The type of graph asset to provide access to. Must derive from GenericBufferAsset
 * @tparam DescriptorType The descriptor type to use for this binding
 * @ingroup Renderer
 */
template<typename TAsset, VkDescriptorType DescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER>
class AssetBufferBinding : public Binding {
public:
    /**
     * @brief Creates the binding
     */
    AssetBufferBinding()
    : Binding(DescriptorType) {}

private:
    using TShaderInput = sr::AssetBufferShaderResource<TAsset>;
    TShaderInput* input;

    virtual DescriptorSetInstance::EntityBindMode getBindMode() const override {
        return DescriptorSetInstance::EntityBindMode::Bindless;
    }
    virtual DescriptorSetInstance::SpeedBucketSetting getSpeedMode() const override {
        return DescriptorSetInstance::SpeedBucketSetting::SpeedAgnostic;
    }

    virtual void init(vk::VulkanState&, sr::ShaderResourceStore& globalShaderResources,
                      sr::ShaderResourceStore& sceneShaderResources,
                      sr::ShaderResourceStore& observerShaderResources) override {
        // TODO - this binding can probably just be removed entirely
        input = sceneShaderResources.getShaderResource<TShaderInput>();
    }

    virtual void writeSet(SetWriteHelper& writer, VkDescriptorSet set, UpdateSpeed,
                          std::uint32_t) override {
        if (input) {
            VkDescriptorBufferInfo& info = writer.getNewBufferInfo();
            info.buffer                  = input->getBuffer().getBuffer();
            info.offset                  = 0;
            info.range                   = input->getBuffer().getSize();

            VkWriteDescriptorSet& write = writer.getNewSetWrite(set);
            write.descriptorType        = DescriptorType;
            write.dstBinding            = getBindingIndex();
            write.pBufferInfo           = &info;
        }
    }

    virtual bool allocateObject(ecs::Entity, scene::Key) override { return true; }

    virtual void releaseObject(ecs::Entity, scene::Key) override {}

    virtual void onFrameStart() override {}

    virtual bool staticDescriptorUpdateRequired() const override {
        return input->staticDescriptorUpdateRequired();
    }

    virtual bool dynamicDescriptorUpdateRequired() const override {
        return input->dynamicDescriptorUpdateRequired();
    }

    template<typename... TBindings>
    friend class Bindings;
};

} // namespace ds
} // namespace rc
} // namespace bl

#endif