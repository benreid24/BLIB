#ifndef BLIB_RENDER_SYSTEMS_TRANSFORM3DSYSTEM_HPP
#define BLIB_RENDER_SYSTEMS_TRANSFORM3DSYSTEM_HPP

#include <BLIB/ECS/ComponentPool.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Transforms/3D/Transform3D.hpp>

namespace bl
{
namespace render
{
namespace sys
{
/**
 * @brief System to recompute dirty transforms into scene buffers as needed
 *
 * @ingroup Renderer
 */
class Transform3DSystem : public engine::System {
public:
    /**
     * @brief Destroys the system
     */
    virtual ~Transform3DSystem() = default;

    /**
     * @brief Recomputes dirty transforms that are also in scene buffers
     *
     * @param Unused
     * @param Unused
     */
    virtual void update(std::mutex&, float) override;

    /**
     * @brief Initializes the system
     *
     * @param engine The game engine instance
     */
    virtual void init(engine::Engine& engine) override;

private:
    ecs::ComponentPool<t3d::Transform3D>* pool;
};

} // namespace sys
} // namespace render
} // namespace bl

#endif
