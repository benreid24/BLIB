#ifndef BLIB_ASSETS_BUILTIN_CUBEMAPPAYLOAD_HPP
#define BLIB_ASSETS_BUILTIN_CUBEMAPPAYLOAD_HPP

#include <BLIB/Assets/Builtin/TexturePayload.hpp>
#include <BLIB/Assets/Dependency.hpp>

namespace bl
{
namespace asi
{
/**
 * @brief Collection of textures representing the faces of a cubemap
 *
 * @ingroup Assets
 */
struct CubemapPayload : public as::Payload {
    /**
     * @brief Create params for cubemap assets
     *
     * @ingroup Assets
     */
    struct CreateParams : public as::CreateContext::CreateData {
        util::UUID top;
        util::UUID bottom;
        util::UUID left;
        util::UUID right;
        util::UUID front;
        util::UUID back;

        /**
         * @brief Creates empty create params
         */
        CreateParams() = default;

        /**
         * @brief Creates from the given texture UUIDs
         *
         * @param top The UUID of the texture for the top face
         * @param bottom The UUID of the texture for the bottom face
         * @param left The UUID of the texture for the left face
         * @param right The UUID of the texture for the right face
         * @param front The UUID of the texture for the front face
         * @param back The UUID of the texture for the back face
         */
        CreateParams(util::UUID top, util::UUID bottom, util::UUID left, util::UUID right,
                     util::UUID front, util::UUID back)
        : top(top)
        , bottom(bottom)
        , left(left)
        , right(right)
        , front(front)
        , back(back) {}

        /**
         * @brief Creates from the given texture refs
         *
         * @param top The texture for the top face
         * @param bottom The texture for the bottom face
         * @param left The texture for the left face
         * @param right The texture for the right face
         * @param front The texture for the front face
         * @param back The texture for the back face
         */
        CreateParams(as::TypedRef<TexturePayload> top, as::TypedRef<TexturePayload> bottom,
                     as::TypedRef<TexturePayload> left, as::TypedRef<TexturePayload> right,
                     as::TypedRef<TexturePayload> front, as::TypedRef<TexturePayload> back)
        : top(top.getUUID())
        , bottom(bottom.getUUID())
        , left(left.getUUID())
        , right(right.getUUID())
        , front(front.getUUID())
        , back(back.getUUID()) {}
    };

    as::Dependency<TexturePayload> top;
    as::Dependency<TexturePayload> bottom;
    as::Dependency<TexturePayload> left;
    as::Dependency<TexturePayload> right;
    as::Dependency<TexturePayload> front;
    as::Dependency<TexturePayload> back;

    /**
     * @brief Creates an empty cubemap payload
     *
     * @param ctx The context to construct with
     */
    CubemapPayload(const as::Payload::ConstructContext& ctx);
};

} // namespace asi
} // namespace bl

#endif
