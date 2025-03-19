#ifndef BLIB_MODELS_TEXTURE_HPP
#define BLIB_MODELS_TEXTURE_HPP

#include <BLIB/Util/HashCombine.hpp>
#include <SFML/Graphics/Image.hpp>
#include <assimp/texture.h>
#include <string>
#include <variant>

namespace bl
{
namespace mdl
{
/**
 * @brief Representation of a texture loaded from a model. Can reference a file path or contain
 *        embedded image data
 *
 * @ingroup Models
 */
class Texture {
public:
    /**
     * @brief Creates an empty texture
     */
    Texture();

    /**
     * @brief Creates the texture from the raw data
     *
     * @param width The width of the image
     * @param height The height of the image
     * @param data The pixels of the image
     */
    void makeFromRaw(unsigned int width, unsigned int height, const aiTexel* data);

    /**
     * @brief Creates the texture from a file path
     *
     * @param file The file path to reference
     */
    void makeFromFile(const std::string& file);

    /**
     * @brief Returns whether this texture contains embedded image data
     */
    bool isEmbedded() const;

    /**
     * @brief Returns the file path referenced by this texture
     */
    const std::string& getFilePath() const;

    /**
     * @brief Returns the embedded image
     */
    const sf::Image& getEmbedded() const;

    /**
     * @brief Texts whether this texture is the same as another
     *
     * @param other The other texture to compare to
     * @return True if the textures are the same, false otherwise
     */
    bool operator==(const Texture& other) const {
        if (isEmbedded() != other.isEmbedded()) return false;
        if (isEmbedded()) { return false; }
        return getFilePath() == other.getFilePath();
    }

private:
    std::variant<std::string, sf::Image> texture;
};

} // namespace mdl
} // namespace bl

namespace std
{
template<>
struct hash<bl::mdl::Texture> {
    std::size_t operator()(const bl::mdl::Texture& tex) const {
        if (tex.isEmbedded()) {
            const sf::Image& img = tex.getEmbedded();
            std::size_t result   = bl::util::hashCombine(std::hash<unsigned int>{}(img.getSize().x),
                                                       std::hash<unsigned int>{}(img.getSize().y));
            for (unsigned int x = 0; x < std::min(img.getSize().x, 10u); x++) {
                for (unsigned int y = 0; y < std::min(img.getSize().y, 10u); ++y) {
                    result = bl::util::hashCombine(
                        result, std::hash<sf::Uint32>{}(img.getPixel(x, y).toInteger()));
                }
            }
            return result;
        }
        return std::hash<std::string>{}(tex.getFilePath());
    }
};
} // namespace std

#endif
