#ifndef BLIB_RENDER_SCENES_SCENE3D_HPP
#define BLIB_RENDER_SCENES_SCENE3D_HPP

#include <BLIB/Render/Lighting/Scene3DLighting.hpp>
#include <BLIB/Render/Scenes/BatchedScene.hpp>

namespace bl
{
namespace rc
{
namespace scene
{
/**
 * @brief 3d scene implementation
 *
 * @ingroup Renderer
 */
class Scene3D : public BatchedScene {
public:
    /**
     * @brief Creates the scene
     *
     * @param engine The game engine instance
     */
    Scene3D(engine::Engine& engine);

    /**
     * @brief Destroys the scene
     */
    virtual ~Scene3D() = default;

    /**
     * @brief Returns the lighting for this scene
     */
    lgt::Scene3DLighting& getLighting() { return lighting; }

protected:
    /**
     * @brief Creates a 3d camera
     */
    virtual std::unique_ptr<cam::Camera> createDefaultCamera() override;

    /**
     * @brief Sets the default near and far planes on the given camera
     *
     * @param camera The camera to initialize
     */
    virtual void setDefaultNearAndFarPlanes(cam::Camera& camera) const override;

    /**
     * @brief Syncs the lighting descriptor set
     */
    virtual void onDescriptorSync() override;

private:
    lgt::Scene3DLighting lighting;
};

} // namespace scene
} // namespace rc
} // namespace bl

#endif
