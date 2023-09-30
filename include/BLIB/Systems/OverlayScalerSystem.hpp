#ifndef BLIB_SYSTEMS_OVERLAYSCALER_HPP
#define BLIB_SYSTEMS_OVERLAYSCALER_HPP

#include <BLIB/Components/OverlayScaler.hpp>
#include <BLIB/Components/Transform2D.hpp>
#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Overlays/OverlayObject.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
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
class OverlayScalerSystem
: public engine::System
, public bl::event::Listener<ecs::event::ComponentAdded<rc::ovy::Viewport>,
                             ecs::event::ComponentRemoved<rc::ovy::Viewport>> {
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
    using Optional = ecs::Optional<rc::ovy::Viewport, com::OverlayScaler>;
    using Result   = ecs::ComponentSet<Required, Optional>;

    ecs::Registry* registry;
    std::vector<rc::Overlay*> overlays;
    ecs::Entity ignoredEntity;
    ecs::View<Required, Optional>* view;
    ecs::ComponentPool<com::OverlayScaler>* scalerPool;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;

    // called by Overlay
    void refreshObjectAndChildren(rc::ovy::OverlayObject& obj, const VkViewport& viewport);

    void refreshObjectAndChildren(Result& row, const VkViewport& viewport);
    void refreshEntity(Result& row, const VkViewport& viewport);

    virtual void observe(const ecs::event::ComponentAdded<rc::ovy::Viewport>& event) override;
    virtual void observe(const ecs::event::ComponentRemoved<rc::ovy::Viewport>& event) override;

    friend class bl::rc::Overlay;
};

} // namespace sys
} // namespace bl

#endif
