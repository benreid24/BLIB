#include <BLIB/Models/Texture.hpp>

namespace bl
{
namespace mdl
{
Texture::Texture()
: texture("") {}

void Texture::makeFromRaw(unsigned int width, unsigned int height, const aiTexel* data) {
    sf::Image& img = texture.emplace<sf::Image>();
    if (width > 0 && height > 0) {
        img.resize({width, height});
        for (unsigned int x = 0; x < width; ++x) {
            for (unsigned int y = 0; y < height; ++y) {
                const aiTexel t = data[y * width + x];
                img.setPixel({x, y}, sf::Color(t.r, t.g, t.b, t.a));
            }
        }
    }
    else { img.loadFromMemory(data, width); }
}

void Texture::makeFromFile(const std::string& file, const std::string& modelPath) {
    const std::string rel = util::FileUtil::joinPath(modelPath, file);
    const std::string relBaseName =
        util::FileUtil::joinPath(modelPath, util::FileUtil::getBaseName(file));
    if (util::FileUtil::exists(relBaseName)) { texture = relBaseName; }
    else if (util::FileUtil::exists(rel)) { texture = rel; }
    else { texture = file; }
}

void Texture::makeFromImage(const sf::Image& image) { texture.emplace<sf::Image>(image); }

void Texture::visit(const DependencyVisitor& visitor) {
    if (!isEmbedded() && !getFilePath().empty()) { texture = visitor(getFilePath()); }
}

bool Texture::isEmbedded() const { return std::holds_alternative<sf::Image>(texture); }

const std::string& Texture::getFilePath() const { return std::get<std::string>(texture); }

const sf::Image& Texture::getEmbedded() const { return std::get<sf::Image>(texture); }

} // namespace mdl
} // namespace bl
