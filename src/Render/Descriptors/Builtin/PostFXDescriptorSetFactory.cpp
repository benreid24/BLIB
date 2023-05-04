#include <BLIB/Render/Descriptors/Builtin/PostFXDescriptorSetFactory.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Render/Scenes/PostFX.hpp>

namespace bl
{
namespace render
{
namespace ds
{
PostFXDescriptorSetFactory::PostFXDescriptorSetFactory()
: device(nullptr) {}

PostFXDescriptorSetFactory::~PostFXDescriptorSetFactory() {
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void PostFXDescriptorSetFactory::init(engine::Engine&, Renderer& renderer) {
    device = renderer.vulkanState().device;

    static const auto layoutBindings = PostFX::DescriptorLayoutBindings();
    VkDescriptorSetLayoutCreateInfo createInfo{};
    createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = static_cast<std::uint32_t>(layoutBindings.size());
    createInfo.pBindings    = layoutBindings.data();
    if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &descriptorSetLayout) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create PostFX descriptor set layout");
    }
}
std::unique_ptr<DescriptorSetInstance> PostFXDescriptorSetFactory::createDescriptorSet() const {
    throw std::runtime_error("No instances should be created for PostFXDescriptorSetFactory");
}

//
} // namespace ds
} // namespace render
} // namespace bl
