#ifndef BLIB_RENDER_SCENES_SCENE2D_HPP
#define BLIB_RENDER_SCENES_SCENE2D_HPP

#include <BLIB/Render/Scenes/BatchedScene.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
/**
 * @brief 2d scene implementation
 *
 * @ingroup Renderer
 */
class Scene2D : public BatchedScene {
public:
    /**
     * @brief Creates the scene
     *
     * @param engine The game engine instance
     */
    Scene2D(engine::Engine& engine);

    /**
     * @brief Destroys the scene
     */
    virtual ~Scene2D() = default;

protected:
    /**
     * @brief Creates a 2d camera
     */
    virtual std::unique_ptr<cam::Camera> createDefaultCamera() override;

    /**
     * @brief Sets the default near and far planes on the given camera
     *
     * @param camera The camera to initialize
     */
    virtual void setDefaultNearAndFarPlanes(cam::Camera& camera) const override;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
