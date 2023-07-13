#include <BLIB/Render/Resources/TextureRef.hpp>

#include <BLIB/Render/Resources/TexturePool.hpp>
#include <BLIB/Render/Vulkan/Texture.hpp>

namespace bl
{
namespace rc
{
namespace res
{
TextureRef::TextureRef()
: owner(nullptr)
, texture(nullptr) {}

TextureRef::TextureRef(TexturePool& owner, vk::Texture& texture)
: owner(&owner)
, texture(&texture) {
    addRef();
}

TextureRef::TextureRef(const TextureRef& copy)
: owner(copy.owner)
, texture(copy.texture) {
    addRef();
}

TextureRef::TextureRef(TextureRef&& m)
: owner(m.owner)
, texture(m.texture) {
    m.texture = nullptr;
}

TextureRef::~TextureRef() { release(); }

TextureRef& TextureRef::operator=(const TextureRef& copy) {
    release();

    owner   = copy.owner;
    texture = copy.texture;
    addRef();
    return *this;
}

TextureRef& TextureRef::operator=(TextureRef&& copy) {
    release();

    owner        = copy.owner;
    texture      = copy.texture;
    copy.texture = nullptr;
    return *this;
}

void TextureRef::addRef() {
    const std::size_t i = id();
    ++owner->refCounts[i];
}

void TextureRef::release() {
    if (!texture) return;

    const std::size_t i = id();
    --owner->refCounts[i];
    texture = nullptr;
    if (owner->refCounts[i].load() == 0) { owner->queueForRelease(i); }
}

std::uint32_t TextureRef::id() const { return texture - &(owner->textures.getTexture(0)); }

TextureRef::operator bool() const { return texture != nullptr; }

} // namespace res
} // namespace rc
} // namespace bl
