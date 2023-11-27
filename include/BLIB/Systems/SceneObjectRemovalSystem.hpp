#ifndef BLIB_SYSTEMS_SCENEOBJECTREMOVALSYSTEM_HPP
#define BLIB_SYSTEMS_SCENEOBJECTREMOVALSYSTEM_HPP

#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Resources/ScenePool.hpp>

namespace bl
{
namespace sys
{
/**
 * @brief Performs object removal in all scenes
 *
 * @ingroup Systems
 */
class SceneObjectRemovalSystem : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    SceneObjectRemovalSystem() = default;

    /**
     * @brief Destroys the system
     */
    virtual ~SceneObjectRemovalSystem() = default;

private:
    rc::res::ScenePool* pool;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex&, float, float, float, float) override;
};

} // namespace sys
} // namespace bl

#endif
