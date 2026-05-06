#ifndef BLIB_GRAPHICS_SKYBOX_HPP
#define BLIB_GRAPHICS_SKYBOX_HPP

#include <BLIB/Assets/Builtin/CubemapPayload.hpp>
#include <BLIB/Assets/TypedRef.hpp>
#include <BLIB/Graphics/Cube.hpp>

namespace bl
{
namespace gfx
{
/**
 * @brief A skybox for 3d scenes
 *
 * @ingroup Graphics
 */
class Skybox {
public:
    /**
     * @brief Does nothing
     */
    Skybox() = default;

    /**
     * @brief Creates the skybox from the 6 faces
     *
     * @param world The world to create the skybox in
     * @param cubemap The cubemap containing the 6 faces of the skybox
     */
    void create(engine::World& world, as::TypedRef<asi::CubemapPayload> cubemap);

    /**
     * @brief Adds this entity to the given scene
     *
     * @param scene The scene to add to
     */
    void addToScene(rc::Scene* scene);

    /**
     * @brief Removes the entity from the scene or overlay that it is in
     */
    void removeFromScene();

    /**
     * @brief Manually issue the required commands to draw this object. Must be called within the
     *        context of a CodeScene
     *
     * @param ctx The CodeScene rendering context
     */
    void draw(rc::scene::CodeScene::RenderContext& ctx);

private:
    Cube cube;
};

} // namespace gfx
} // namespace bl

#endif
