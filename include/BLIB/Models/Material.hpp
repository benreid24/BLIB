#ifndef BLIB_MODELS_MATERIAL_HPP
#define BLIB_MODELS_MATERIAL_HPP

#include <BLIB/Models/Texture.hpp>
#include <string>

namespace bl
{
namespace mdl
{
/**
 * @brief Basic struct with material info for a mesh
 *
 * @ingroup Models
 */
struct Material {
    Texture diffuse;
    Texture normal;
    Texture specular;
    Texture parallax;
    float shininess;
    float heightScale;

    /**
     * @brief Initializes the material with sane defaults
     */
    Material()
    : shininess(0.5f)
    , heightScale(0.f) {}

    /**
     * @brief Tests whether the material is the same as another
     *
     * @param other The material to compare to
     * @return True if the materials are the same, false otherwise
     */
    bool operator==(const Material& other) const {
        return diffuse == other.diffuse && normal == other.normal && specular == other.specular &&
               parallax == other.parallax && shininess == other.shininess &&
               heightScale == other.heightScale;
    }
};

} // namespace mdl
} // namespace bl

namespace std
{
template<>
struct hash<bl::mdl::Material> {
    std::size_t operator()(const bl::mdl::Material& mat) const {
        std::size_t hash = 0;
        hash             = bl::util::hashCombine(hash, std::hash<bl::mdl::Texture>{}(mat.diffuse));
        hash             = bl::util::hashCombine(hash, std::hash<bl::mdl::Texture>{}(mat.normal));
        hash             = bl::util::hashCombine(hash, std::hash<bl::mdl::Texture>{}(mat.specular));
        hash             = bl::util::hashCombine(hash, std::hash<bl::mdl::Texture>{}(mat.parallax));
        hash             = bl::util::hashCombine(hash, std::hash<float>{}(mat.shininess));
        hash             = bl::util::hashCombine(hash, std::hash<float>{}(mat.heightScale));
        return hash;
    }
};
} // namespace std

#endif
