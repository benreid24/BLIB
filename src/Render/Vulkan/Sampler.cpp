#include <BLIB/Render/Vulkan/Sampler.hpp>

#include <BLIB/Render/Vulkan/VulkanLayer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
Sampler::Sampler()
: vulkanState(nullptr)
, sampler(nullptr)
, owner(false) {}

Sampler::Sampler(VulkanLayer& vs, VkSampler sampler, bool owner)
: vulkanState(&vs)
, sampler(sampler)
, owner(owner) {}

Sampler::Sampler(Sampler&& ms)
: vulkanState(ms.vulkanState)
, sampler(ms.sampler)
, owner(ms.owner) {
    if (ms.owner) {
        ms.vulkanState = nullptr;
        ms.sampler     = nullptr;
        ms.owner       = false;
    }
}

Sampler::~Sampler() { release(); }

Sampler& Sampler::operator=(Sampler&& ms) {
    release();

    vulkanState = ms.vulkanState;
    sampler     = ms.sampler;
    owner       = ms.owner;

    if (ms.owner) {
        ms.vulkanState = nullptr;
        ms.sampler     = nullptr;
        ms.owner       = false;
    }

    return *this;
}

void Sampler::release() {
    if (sampler && owner) {
        vkDestroySampler(vulkanState->getDevice(), sampler, nullptr);
        vulkanState = nullptr;
        sampler     = nullptr;
        owner       = false;
    }
}

void Sampler::deferRelease() {
    if (sampler && owner) {
        vulkanState->getCleanupManager().add(
            [s = sampler, d = vulkanState->getDevice()]() { vkDestroySampler(d, s, nullptr); });
        vulkanState = nullptr;
        sampler     = nullptr;
        owner       = false;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl