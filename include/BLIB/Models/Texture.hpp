#ifndef BLIB_MODELS_TEXTURE_HPP
#define BLIB_MODELS_TEXTURE_HPP

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
     * @brief Returns the width of the embedded image
     */
    unsigned int getEmbeddedWidth() const;

    /**
     * @brief Returns the height of the embedded image
     */
    unsigned int getEmbeddedHeight() const;

    /**
     * @brief Returns the data of the embedded image
     */
    const aiTexel* getEmbeddedData() const;

private:
    struct Raw {
        unsigned int width;
        unsigned int height;
        const aiTexel* data;

        Raw()
        : width(0)
        , height(0)
        , data(nullptr) {}

        Raw(unsigned int w, unsigned int h, const aiTexel* d)
        : width(w)
        , height(h)
        , data(d) {}
    };

    std::variant<std::string, Raw> texture;
};

} // namespace mdl
} // namespace bl

#endif
