#include <BLIB/Render/Vulkan/TextureBase.hpp>

#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace rc
{
namespace vk
{
TextureBase::TextureBase()
: parent(nullptr)
, sampler(nullptr)
, sizeRaw(0, 0)
, sizeF(0.f, 0.f)
, hasTransparency(false) {}

glm::vec2 TextureBase::convertCoord(const glm::vec2& src) const {
    // TODO - texture atlasing at the renderer level
    return src;
}

glm::vec2 TextureBase::normalizeAndConvertCoord(const glm::vec2& src) const {
    return convertCoord(src / size());
}

void TextureBase::setSampler(VkSampler s) {
    sampler = s;
    parent->updateTexture(this);
}

void TextureBase::ensureSize(const glm::u32vec2& s) {
    if (s.x <= rawSize().x && s.y <= rawSize().y) return;

    resize(s);
}

void TextureBase::updateSize(const glm::u32vec2& s) {
    sizeRaw = s;
    sizeF.x = static_cast<float>(sizeRaw.x);
    sizeF.y = static_cast<float>(sizeRaw.y);
}

void TextureBase::updateTransparency(bool t) { hasTransparency = t; }

void TextureBase::updateDescriptors() { parent->updateTexture(this); }

} // namespace vk
} // namespace rc
} // namespace bl
