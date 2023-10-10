#ifndef BLIB_SYSTEMS_OVERLAYSCALER_HPP
#define BLIB_SYSTEMS_OVERLAYSCALER_HPP

#include <BLIB/Components/OverlayScaler.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <vector>

namespace bl
{
namespace rc
{
class Overlay;
}

namespace sys
{
/**
 * @brief System that processes entities that are scaled for overlays. Keeps scales up-to-date as
 *        overlay size changes
 *
 * @ingroup Systems
 */
class OverlayScalerSystem : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    OverlayScalerSystem() = default;

    /**
     * @brief Destroys the system
     */
    virtual ~OverlayScalerSystem() = default;

private:
    using Required = ecs::Require<rc::ovy::OverlayObject, com::Transform2D>;
    using Optional = ecs::Optional<com::OverlayScaler>;
    using Result   = ecs::ComponentSet<Required, Optional>;

    ecs::Registry* registry;
    std::vector<rc::Overlay*> overlays;
    ecs::View<Required, Optional>* view;
    ecs::ComponentPool<com::OverlayScaler>* scalerPool;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt, float, float, float) override;

    // called by Overlay
    void refreshObjectAndChildren(rc::ovy::OverlayObject& obj);

    void refreshObjectAndChildren(Result& row);
    void refreshEntity(Result& row);

    friend class bl::rc::Overlay;
};

} // namespace sys
} // namespace bl

#endif
