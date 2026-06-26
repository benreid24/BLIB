#ifndef BLIB_ASSETS_BUILTIN_TEXTUREPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_TEXTUREPAYLOAD_HPP

#include <BLIB/Assets/Builtin/ImagePayload.hpp>
#include <BLIB/Assets/Context.hpp>
#include <BLIB/Assets/Dependency.hpp>
#include <BLIB/Assets/Payload.hpp>
#include <BLIB/Reflection/ReflectedObject.hpp>
#include <BLIB/Serialization.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Payload for texture assets. Textures depend on an image asset and provide additional
 *        metadata for rendering
 *
 * @ingroup Assets
 */
struct TexturePayload : public as::Payload {
    /// Represents the role of the texture
    enum Type : std::uint8_t {
        Generic         = 0,
        Diffuse         = 1,
        NormalMap       = 2,
        SpecularMap     = 3,
        DisplacementMap = 4
    };

    /// Represents the color space of the image
    enum ColorSpace : std::uint8_t { Linear = 0, sRGB = 1 };

    /**
     * @brief Creation parameters for texture assets
     *
     * @ingroup Assets
     */
    struct CreateData : public as::CreateContext::CreateData {
        /// Mutually exclusive with path. The UUID of an existing image asset to use
        util::UUID image;
        Type type;
        ColorSpace colorSpace;

        /**
         * @brief Creates empty create data
         */
        CreateData();

        /**
         * @brief Creates from the underlying image file path. Will find or create image asset
         *
         * @param path Source path for the image
         */
        CreateData(const std::string& path);

        /**
         * @brief Creates from an existing image asset
         *
         * @param image The UUID of the existing image asset to use
         */
        CreateData(util::UUID image);
    };

    /**
     * @brief Creates the payload for texture assets
     *
     * @param ctx The context to construct with
     */
    TexturePayload(const Payload::ConstructContext& ctx);

    /**
     * @brief Destroys the payload
     */
    virtual ~TexturePayload() = default;

    as::Dependency<ImagePayload> image;
    Type type;
    ColorSpace colorSpace;
};

} // namespace asi

namespace serial
{
template<>
struct SerializableObject<asi::TexturePayload> : public SerializableObjectBase {
    SerializableField<1, asi::TexturePayload, asi::TexturePayload::Type> type;
    SerializableField<2, asi::TexturePayload, asi::TexturePayload::ColorSpace> colorSpace;

    SerializableObject()
    : SerializableObjectBase("Texture")
    , type("type", *this, &asi::TexturePayload::type, SerializableFieldBase::Required{})
    , colorSpace("colorSpace", *this, &asi::TexturePayload::colorSpace,
                 SerializableFieldBase::Required{}) {}
};
} // namespace serial

namespace refl
{
template<>
struct ReflectedObject<asi::TexturePayload> {
    inline static const auto spec = makeSpec<asi::TexturePayload>(
        "Texture", memberList(defineMember(1, "type", &asi::TexturePayload::type),
                              defineMember(2, "colorSpace", &asi::TexturePayload::colorSpace)));
};
} // namespace refl

} // namespace bl

#endif
