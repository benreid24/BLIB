#include <BLIB/Render/Vulkan/DescriptorPoolRequirements.hpp>

namespace bl
{
namespace render
{
DescriptorPoolRequirements::DescriptorPoolRequirements()
: maxSets(0) {
    descriptorCounts.reserve(12);
}

void DescriptorPoolRequirements::addSets(std::uint32_t c) { maxSets += c; }

void DescriptorPoolRequirements::registerDescriptorType(VkDescriptorType t, std::uint32_t c) {
    for (VkDescriptorPoolSize& ps : descriptorCounts) {
        if (ps.type == t) {
            ps.descriptorCount += c;
            return;
        }
    }
    descriptorCounts.emplace_back();
    descriptorCounts.back().type            = t;
    descriptorCounts.back().descriptorCount = c;
}

VkDescriptorPoolCreateInfo DescriptorPoolRequirements::buildRequirements() const {
    VkDescriptorPoolCreateInfo ci;
    ci.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    // TODO - add any extra buffer?
    ci.maxSets       = maxSets;
    ci.pPoolSizes    = descriptorCounts.data();
    ci.poolSizeCount = descriptorCounts.size();
    return ci;
}

} // namespace render
} // namespace bl
