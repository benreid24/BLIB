#include <BLIB/Render/Renderer/Texture.hpp>

namespace bl
{
namespace render
{
Texture::Texture()
: textureImage(nullptr)
, textureImageMemory(nullptr)
, textureImageView(nullptr)
, textureSampler(nullptr) {}

Texture::~Texture() {
    // TODO
}

} // namespace render
} // namespace bl
