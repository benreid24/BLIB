#ifndef BLIB_MODELS_MATERIAL_HPP
#define BLIB_MODELS_MATERIAL_HPP

#include <BLIB/Models/Texture.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>
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

namespace serial
{
template<>
struct SerializableObject<bl::mdl::Material> : public SerializableObjectBase {
    SerializableField<1, bl::mdl::Material, mdl::Texture> diffuse;
    SerializableField<2, bl::mdl::Material, mdl::Texture> normal;
    SerializableField<3, bl::mdl::Material, mdl::Texture> specular;
    SerializableField<4, bl::mdl::Material, mdl::Texture> parallax;
    SerializableField<5, bl::mdl::Material, float> shininess;
    SerializableField<6, bl::mdl::Material, float> heightScale;

    SerializableObject()
    : SerializableObjectBase("Material")
    , diffuse("diffuse", *this, &bl::mdl::Material::diffuse, SerializableFieldBase::Required{})
    , normal("normal", *this, &bl::mdl::Material::normal, SerializableFieldBase::Required{})
    , specular("specular", *this, &bl::mdl::Material::specular, SerializableFieldBase::Required{})
    , parallax("parallax", *this, &bl::mdl::Material::parallax, SerializableFieldBase::Required{})
    , shininess("shininess", *this, &bl::mdl::Material::shininess,
                SerializableFieldBase::Required{})
    , heightScale("heightScale", *this, &bl::mdl::Material::heightScale,
                  SerializableFieldBase::Required{}) {}
};

} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<mdl::Material> {
    inline static const auto spec = makeSpec<mdl::Material>(
        "Material", memberList(defineMember(1, "diffuse", &mdl::Material::diffuse),
                               defineMember(2, "normal", &mdl::Material::normal),
                               defineMember(3, "specular", &mdl::Material::specular),
                               defineMember(4, "parallax", &mdl::Material::parallax),
                               defineMember(5, "shininess", &mdl::Material::shininess),
                               defineMember(6, "heightScale", &mdl::Material::heightScale)));
};
} // namespace refl

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
