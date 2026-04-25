#ifndef BLIB_ASSETS_BUILTIN_MATERIALPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_MATERIALPAYLOAD_HPP

#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Models/Material.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload type for 3d materials
 *
 * @ingroup Assets
 */
struct MaterialPayload : public as::Payload {
    as::Dependency<TexturePayload> diffuse;
    as::Dependency<TexturePayload> normal;
    as::Dependency<TexturePayload> specular;
    as::Dependency<TexturePayload> parallax;
    float shininess;
    float heightScale;

    /**
     * @brief Custom create params for material payloads
     *
     * @ingroup Assets
     */
    struct CreateData : public as::CreateContext::CreateData {
        const mdl::Material& material;

        /**
         * @brief Creates the create data with the given material
         *
         * @param material The material definition
         */
        CreateData(const mdl::Material& material);
    };

    /**
     * @brief Creates the material payload
     *
     * @param ctx The context to construct with
     */
    MaterialPayload(const Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~MaterialPayload() = default;
};

} // namespace asi

namespace serial
{
template<>
struct SerializableObject<asi::MaterialPayload> : public SerializableObjectBase {
    SerializableField<1, asi::MaterialPayload, float> shininess;
    SerializableField<2, asi::MaterialPayload, float> heightScale;

    SerializableObject()
    : SerializableObjectBase("MaterialPayload")
    , shininess("shininess", *this, &asi::MaterialPayload::shininess,
                SerializableFieldBase::Required{})
    , heightScale("heightScale", *this, &asi::MaterialPayload::heightScale,
                  SerializableFieldBase::Required{}) {}
};
} // namespace serial

} // namespace bl

#endif
