#include <BLIB/Render/Vulkan/Sampler.hpp>

#include <BLIB/Render/Renderer.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
Sampler::Sampler()
: renderer(nullptr)
, sampler(nullptr)
, owner(false) {}

Sampler::Sampler(Renderer& renderer, VkSampler sampler, bool owner)
: renderer(&renderer)
, sampler(sampler)
, owner(owner) {}

Sampler::Sampler(Sampler&& ms)
: renderer(ms.renderer)
, sampler(ms.sampler)
, owner(ms.owner) {
    if (ms.owner) {
        ms.renderer = nullptr;
        ms.sampler  = nullptr;
        ms.owner    = false;
    }
}

Sampler::~Sampler() { release(); }

Sampler& Sampler::operator=(Sampler&& ms) {
    release();

    renderer = ms.renderer;
    sampler  = ms.sampler;
    owner    = ms.owner;

    if (ms.owner) {
        ms.renderer = nullptr;
        ms.sampler  = nullptr;
        ms.owner    = false;
    }

    return *this;
}

void Sampler::release() {
    if (sampler && owner) {
        vkDestroySampler(renderer->vulkanState().getDevice(), sampler, nullptr);
        renderer = nullptr;
        sampler  = nullptr;
        owner    = false;
    }
}

void Sampler::deferRelease() {
    if (sampler && owner) {
        renderer->getCleanupManager().add([s = sampler, d = renderer->vulkanState().getDevice()]() {
            vkDestroySampler(d, s, nullptr);
        });
        renderer = nullptr;
        sampler  = nullptr;
        owner    = false;
    }
}

} // namespace vk
} // namespace rc
} // namespace bl