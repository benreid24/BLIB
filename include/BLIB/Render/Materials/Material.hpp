#ifndef BLIB_RENDER_MATERIALS_MATERIAL_HPP
#define BLIB_RENDER_MATERIALS_MATERIAL_HPP

#include <BLIB/Render/Resources/TextureRef.hpp>

namespace bl
{
namespace rc
{
namespace mat
{
/**
 * @brief Stores all data for a material
 *
 * @ingroup Renderer
 */
class Material {
public:
    static constexpr float DefaultShininess = 0.5f;

    /**
     * @brief Creates an empty material
     */
    Material();

    /**
     * @brief Copies the material
     */
    Material(const Material&) = default;

    /**
     * @brief Copies the material
     */
    Material(Material&&) = default;

    /**
     * @brief Creates a material from its components
     *
     * @param diffuse The diffuse texture the material should use
     * @param specular The specular texture the material should use
     * @param normal The normal map for the texture
     * @param shininess The shininess parameter of the texture
     */
    Material(const res::TextureRef& diffuse, const res::TextureRef& specular,
             const res::TextureRef& normal, float shininess = DefaultShininess);

    /**
     * @brief Destroys the material
     */
    ~Material() = default;

    /**
     * @brief Copies the material
     */
    Material& operator=(const Material&) = default;

    /**
     * @brief Copies the material
     */
    Material& operator=(Material&&) = default;

    /**
     * @brief Returns the diffuse texture of this material
     */
    const res::TextureRef& getTexture() const { return texture; }

    /**
     * @brief Returns the normal texture of this material
     */
    const res::TextureRef& getNormalMap() const { return normalMap; }

    /**
     * @brief Returns the specular texture of this material
     */
    const res::TextureRef& getSpecularMap() const { return specularMap; }

    /**
     * @brief Returns the shininess of the material
     */
    float getShininess() const { return shininess; }

private:
    res::TextureRef texture;
    res::TextureRef normalMap;
    res::TextureRef specularMap;
    float shininess;
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
