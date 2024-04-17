#ifndef DESCRIPTORFACTORY_HPP
#define DESCRIPTORFACTORY_HPP

#include "DescriptorSet.hpp"
#include <BLIB/Render/Descriptors/DescriptorSetFactory.hpp>

class DescriptorFactory : public bl::rc::ds::DescriptorSetFactory {
public:
    virtual ~DescriptorFactory() = default;

private:
    bl::engine::Engine* engine;

    virtual void init(bl::engine::Engine& e, bl::rc::Renderer& renderer) override {
        engine = &e;

        bl::rc::vk::DescriptorPool::SetBindingInfo bindings;
        bindings.bindingCount = 1;

        bindings.bindings[0].binding         = 0;
        bindings.bindings[0].descriptorCount = 1;
        bindings.bindings[0].descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings.bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        descriptorSetLayout = renderer.vulkanState().descriptorPool.createLayout(bindings);
    }

    virtual std::unique_ptr<bl::rc::ds::DescriptorSetInstance> createDescriptorSet()
        const override {
        return std::make_unique<DescriptorSet>(*engine, descriptorSetLayout);
    }
};

#endif
