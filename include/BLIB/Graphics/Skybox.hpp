#ifndef BLIB_GRAPHICS_SKYBOX_HPP
#define BLIB_GRAPHICS_SKYBOX_HPP

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
     * @param right The right face image
     * @param left The left face image
     * @param top The top face image
     * @param bottom The bottom face image
     * @param back The back face image
     * @param front The front face image
     */
    void create(engine::World& world, const std::string& right, const std::string& left,
                const std::string& top, const std::string& bottom, const std::string& back,
                const std::string& front);

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
