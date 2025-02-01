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
     * @brief Creates a material from a single texture
     *
     * @param texture The texture to create the material from
     */
    Material(const res::TextureRef& texture);

    /**
     * @brief Returns the diffuse texture of this material
     */
    const res::TextureRef& getTexture() const { return texture; }

    /**
     * @brief Returns the normal texture of this material
     */
    const res::TextureRef& getNormalMap() const { return normalMap; }

    /**
     * @brief Returns the uv texture of this material
     */
    const res::TextureRef& getUVMap() const { return uvMap; }

private:
    res::TextureRef texture;
    res::TextureRef normalMap;
    res::TextureRef uvMap;
    // TODO - other parameters?
};

} // namespace mat
} // namespace rc
} // namespace bl

#endif
