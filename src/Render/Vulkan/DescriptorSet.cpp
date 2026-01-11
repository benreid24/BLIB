#include <BLIB/Render/Vulkan/DescriptorSet.hpp>

#include <BLIB/Render/Renderer.hpp>
#include <BLIB/Util/Random.hpp>
#include <unordered_map>

namespace bl
{
namespace rc
{
namespace vk
{
DescriptorSet::DescriptorSet()
: renderer(nullptr)
, set(nullptr) {}

DescriptorSet::DescriptorSet(Renderer& r)
: renderer(&r)
, set(nullptr) {}

DescriptorSet::DescriptorSet(Renderer& r, VkDescriptorSetLayout layout, bool ded)
: DescriptorSet(r) {
    allocate(layout, ded);
}

DescriptorSet::DescriptorSet(DescriptorSet&& ds)
: renderer(ds.renderer)
, alloc(std::move(ds.alloc))
, set(ds.set) {
    ds.set = nullptr;
}

DescriptorSet::~DescriptorSet() { release(); }

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& ds) {
    alloc  = std::move(ds.alloc);
    set    = ds.set;
    ds.set = nullptr;
    return *this;
}

void DescriptorSet::init(Renderer& r) { renderer = &r; }

void DescriptorSet::allocate(VkDescriptorSetLayout layout, bool dedicated) {
    deferRelease();
    alloc = renderer->getDescriptorPool().allocate(layout, &set, 1, dedicated);
}

void DescriptorSet::release() {
    if (set) {
        renderer->getDescriptorPool().release(alloc);
        set = nullptr;
    }
}

void DescriptorSet::deferRelease() {
    if (set) {
        renderer->getCleanupManager().add(
            [alloc = alloc, set = set]() mutable { alloc.release(&set); });
        set = nullptr;
    }
}

void DescriptorSet::bind(VkCommandBuffer commandBuffer, VkPipelineBindPoint bindPoint,
                         VkPipelineLayout layout, std::uint32_t index) {
    vkCmdBindDescriptorSets(commandBuffer, bindPoint, layout, index, 1, &set, 0, nullptr);
}

} // namespace vk
} // namespace rc
} // namespace bl
