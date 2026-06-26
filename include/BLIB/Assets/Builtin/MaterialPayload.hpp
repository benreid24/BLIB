#ifndef BLIB_ASSETS_BUILTIN_MATERIALPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_MATERIALPAYLOAD_HPP

#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Models/Material.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>

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
    as::Dependency<TexturePayload, as::LoadPolicy::Eager, as::DependencyPolicy::Optional> diffuse;
    as::Dependency<TexturePayload, as::LoadPolicy::Eager, as::DependencyPolicy::Optional> normal;
    as::Dependency<TexturePayload, as::LoadPolicy::Eager, as::DependencyPolicy::Optional> specular;
    as::Dependency<TexturePayload, as::LoadPolicy::Eager, as::DependencyPolicy::Optional> parallax;
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

namespace refl
{
template<>
struct ReflectedObject<asi::MaterialPayload> {
    inline static const auto spec = makeSpec<asi::MaterialPayload>(
        "MaterialPayload",
        memberList(defineMember(1, "shininess", &asi::MaterialPayload::shininess),
                   defineMember(2, "heightScale", &asi::MaterialPayload::heightScale)));
};
} // namespace refl

} // namespace bl

#endif
