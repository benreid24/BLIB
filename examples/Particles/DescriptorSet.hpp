#ifndef DESCRIPTORSET_HPP
#define DESCRIPTORSET_HPP

// TODO - remove when templated and in render module
#include "Particle.hpp"
#include <BLIB/Engine/Engine.hpp>

#include <BLIB/Particles/Link.hpp>
#include <BLIB/Render/Buffers/FullyDynamicSSBO.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>

/**
 * TODO: Template payload and particle type. Overload operator= if different
 */

class DescriptorSet : public bl::rc::ds::DescriptorSetInstance {
public:
    DescriptorSet(bl::engine::Engine& engine, VkDescriptorSetLayout layout)
    : DescriptorSetInstance(Bindful, SpeedAgnostic)
    , engine(engine)
    , layout(layout) {}

    virtual ~DescriptorSet() = default;

private:
    struct Instance {
        const VkDevice device;
        const VkDescriptorSetLayout layout;
        bl::rc::buf::FullyDynamicSSBO<Particle> storage;
        bl::rc::vk::PerFrame<bl::rc::vk::DescriptorSet> descriptorSets;
        bl::pcl::Link<Particle>* link;

        Instance(bl::engine::Engine& engine, VkDescriptorSetLayout layout, bl::ecs::Entity entity)
        : device(engine.renderer().vulkanState().device)
        , layout(layout) {
            storage.create(engine.renderer().vulkanState(), 128);
            descriptorSets.init(engine.renderer().vulkanState(),
                                [this, &engine, layout](bl::rc::vk::DescriptorSet& set) {
                                    set.init(engine.renderer().vulkanState());
                                    writeDescriptorSet(set);
                                });
            link = engine.ecs().getComponent<bl::pcl::Link<Particle>>(entity);
            if (!link) {
                BL_LOG_CRITICAL << "Link component not created for ParticleSystem: " << entity;
                throw std::runtime_error("Link component not created for ParticleSystem");
            }
        }

        void writeDescriptorSet(bl::rc::vk::DescriptorSet& set) {
            set.allocate(layout);

            bl::rc::vk::Buffer& buffer = descriptorSets.getOther(set, storage.gpuBufferHandles());

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = buffer.getBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range  = buffer.getSize();

            VkWriteDescriptorSet setWrite{};
            setWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            setWrite.descriptorCount = 1;
            setWrite.dstBinding      = 0;
            setWrite.dstArrayElement = 0;
            setWrite.dstSet          = set.getSet();
            setWrite.pBufferInfo     = &bufferInfo;
            setWrite.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // TODO - UBO instead?

            vkUpdateDescriptorSets(device, 1, &setWrite, 0, nullptr);
        }

        void copyData() {
            if (storage.performFullCopy(link->base, link->len)) {
                descriptorSets.visit(
                    [this](bl::rc::vk::DescriptorSet& ds) { writeDescriptorSet(ds); });
            }
        }
    };

    bl::engine::Engine& engine;
    const VkDescriptorSetLayout layout;
    mutable std::unordered_map<std::uint32_t, Instance> instances;

    virtual void init(bl::rc::ds::DescriptorComponentStorageCache&) override {}
    virtual void bindForPipeline(bl::rc::scene::SceneRenderContext&, VkPipelineLayout,
                                 std::uint32_t, bl::rc::UpdateSpeed) const override {}

    virtual void bindForObject(bl::rc::scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex,
                               bl::rc::scene::Key objectKey) const override {
        const auto it = instances.find(objectKey.sceneId);
        if (it == instances.end()) {
            BL_LOG_ERROR << "Failed to find instance for ParticleSystem " << objectKey.sceneId;
            return;
        }

        if (it->second.storage.ensureSize(it->second.link->len, true)) {
            it->second.writeDescriptorSet(it->second.descriptorSets.current());
        }

        it->second.descriptorSets.current().bind(
            ctx.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, layout, setIndex);
    }

    virtual bool allocateObject(bl::ecs::Entity entity, bl::rc::scene::Key key) override {
        if (key.updateFreq != bl::rc::UpdateSpeed::Dynamic) {
            BL_LOG_WARN
                << "All ParticleManagers should use Dynamic scene speed to avoid key collisions";
        }
        if (!instances.try_emplace(key.sceneId, engine, layout, entity).second) {
            BL_LOG_ERROR << "Collision for ParticleManager " << entity << " with scene key "
                         << key.sceneId;
            return false;
        }
        return true;
    }

    virtual void releaseObject(bl::ecs::Entity, bl::rc::scene::Key key) override {
        instances.erase(key.sceneId);
    }

    virtual void handleFrameStart() override {
        for (auto& pair : instances) { pair.second.copyData(); }
    }
};

#endif
