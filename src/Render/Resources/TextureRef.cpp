#include <BLIB/Render/Resources/TextureRef.hpp>

#include <BLIB/Render/Renderer/Texture.hpp>
#include <BLIB/Render/Resources/TexturePool.hpp>

namespace bl
{
namespace render
{
TextureRef::TextureRef(TexturePool& owner, Texture& texture)
: owner(owner)
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

TextureRef::~TextureRef() {
    if (texture) { release(); }
}

TextureRef& TextureRef::operator=(const TextureRef& copy) {
#ifdef BLIB_DEBUG
    if (&owner != &copy.owner) {
        throw std::runtime_error("Cannot copy TextureRef between TexturePools");
    }
#endif

    texture = copy.texture;
    addRef();
    return *this;
}

TextureRef& TextureRef::operator=(TextureRef&& copy) {
#ifdef BLIB_DEBUG
    if (&owner != &copy.owner) {
        throw std::runtime_error("Cannot copy TextureRef between TexturePools");
    }
#endif

    texture      = copy.texture;
    copy.texture = nullptr;
    return *this;
}

void TextureRef::addRef() {
    const std::size_t i = id();
    ++owner.refCounts[i];
}

void TextureRef::release() {
    const std::size_t i = id();
    --owner.refCounts[i];
    texture = nullptr;
    if (owner.refCounts[i].load() == 0) { owner.queueForRelease(i); }
}

std::uint32_t TextureRef::id() const { return texture - owner.textures.data(); }

} // namespace render
} // namespace bl
