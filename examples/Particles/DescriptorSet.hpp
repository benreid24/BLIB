#ifndef DESCRIPTORSET_HPP
#define DESCRIPTORSET_HPP

#include "Particle.hpp"
#include <BLIB/Render/Buffers/DynamicSSBO.hpp>
#include <BLIB/Render/Descriptors/DescriptorSetInstance.hpp>
#include <BLIB/Render/Vulkan/DescriptorPool.hpp>
#include <BLIB/Render/Vulkan/DescriptorSet.hpp>

class DescriptorSet : public bl::rc::ds::DescriptorSetInstance {
public:
    DescriptorSet(bl::engine::Engine& engine);

    virtual ~DescriptorSet() = default;

private:
    struct Instance {
        bl::rc::buf::DynamicSSBO<Particle> storage;
        bl::rc::vk::PerFrame<bl::rc::vk::DescriptorSet> descriptorSets;
    };

    bl::engine::Engine& engine;
    std::unordered_map<std::uint32_t, Instance> instances;

    virtual void init(bl::rc::ds::DescriptorComponentStorageCache&) override {}
    virtual void bindForPipeline(bl::rc::scene::SceneRenderContext&, VkPipelineLayout,
                                 std::uint32_t, bl::rc::UpdateSpeed) const override {}
    virtual void bindForObject(bl::rc::scene::SceneRenderContext& ctx, VkPipelineLayout layout,
                               std::uint32_t setIndex, bl::rc::scene::Key objectKey) const override;
    virtual bool allocateObject(bl::ecs::Entity entity, bl::rc::scene::Key key) override;
    virtual void releaseObject(bl::ecs::Entity entity, bl::rc::scene::Key key) override;
    virtual void handleFrameStart() override;
};

#endif
