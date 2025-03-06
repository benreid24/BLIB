#include <BLIB/Models/Texture.hpp>

namespace bl
{
namespace mdl
{
Texture::Texture()
: texture(Raw()) {}

void Texture::makeFromRaw(unsigned int width, unsigned int height, const aiTexel* data) {
    texture = Raw(width, height, data);
}

void Texture::makeFromFile(const std::string& file) { texture = file; }

bool Texture::isEmbedded() const { return std::holds_alternative<Raw>(texture); }

const std::string& Texture::getFilePath() const { return std::get<std::string>(texture); }

unsigned int Texture::getEmbeddedWidth() const { return std::get<Raw>(texture).width; }

unsigned int Texture::getEmbeddedHeight() const { return std::get<Raw>(texture).height; }

const aiTexel* Texture::getEmbeddedData() const { return std::get<Raw>(texture).data; }

} // namespace mdl
} // namespace bl
