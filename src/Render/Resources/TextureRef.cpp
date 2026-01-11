#include <BLIB/Render/Resources/TextureRef.hpp>

#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>

namespace bl
{
namespace rc
{
namespace res
{
namespace
{
bool rendererAlive = true;
}

TextureRef::TextureRef()
: owner(nullptr)
, texture(nullptr)
, arrayId(0) {}

TextureRef::TextureRef(TexturePool& owner, vk::Texture& texture, std::uint32_t aid)
: owner(&owner)
, texture(&texture)
, arrayId(aid) {
    addRef();
}

TextureRef::TextureRef(const TextureRef& copy)
: owner(copy.owner)
, texture(copy.texture)
, arrayId(copy.arrayId) {
    if (texture) { addRef(); }
}

TextureRef::TextureRef(TextureRef&& m)
: owner(m.owner)
, texture(m.texture)
, arrayId(m.arrayId) {
    m.texture = nullptr;
}

TextureRef::~TextureRef() { release(); }

TextureRef& TextureRef::operator=(const TextureRef& copy) {
    release();

    owner   = copy.owner;
    texture = copy.texture;
    arrayId = copy.arrayId;
    addRef();
    return *this;
}

TextureRef& TextureRef::operator=(TextureRef&& copy) {
    release();

    owner        = copy.owner;
    texture      = copy.texture;
    arrayId      = copy.arrayId;
    copy.texture = nullptr;
    return *this;
}

void TextureRef::addRef() {
    auto& refCounts = texture->getType() == vk::Texture::Type::Cubemap ? owner->cubemapRefCounts :
                                                                         owner->refCounts;
    const std::size_t i = id();
    ++refCounts[i];
}

void TextureRef::release() {
    if (!texture || !rendererAlive) return;

    auto& refCounts = texture->getType() == vk::Texture::Type::Cubemap ? owner->cubemapRefCounts :
                                                                         owner->refCounts;
    const std::size_t i = id();
    --refCounts[i];
    if (refCounts[i].load() == 0) { owner->queueForRelease(texture); }
    texture = nullptr;
}

TextureRef::operator bool() const { return texture != nullptr; }

void TextureRef::disableCleanup() { rendererAlive = false; }

} // namespace res
} // namespace rc
} // namespace bl
