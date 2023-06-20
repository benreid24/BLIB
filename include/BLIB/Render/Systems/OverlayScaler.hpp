#ifndef BLIB_RENDER_SYSTEMS_OVERLAYSCALER_HPP
#define BLIB_RENDER_SYSTEMS_OVERLAYSCALER_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Components/OverlayScaler.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Render/Overlays/Viewport.hpp>
#include <BLIB/Transforms/2D/Transform2D.hpp>
#include <vector>

namespace bl
{
namespace render
{
class Overlay;

namespace sys
{
/**
 * @brief System that processes entities that are scaled for overlays. Keeps scales up-to-date as
 *        overlay size changes
 *
 * @ingroup Renderer
 */
class OverlayScaler
: public engine::System
, public bl::event::Listener<ecs::event::ComponentAdded<ovy::Viewport>,
                             ecs::event::ComponentRemoved<ovy::Viewport>> {
public:
    /**
     * @brief Creates the system
     */
    OverlayScaler() = default;

    /**
     * @brief Destroys the system
     */
    virtual ~OverlayScaler() = default;

private:
    ecs::Registry* registry;
    std::vector<Overlay*> overlays;
    ecs::Entity ignoredEntity;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;

    // called by Overlay
    void registerOverlay(Overlay* overlay);
    void removeOverlay(Overlay* overlay);
    void refreshEntity(ecs::Entity entity, const glm::vec2& targetSize);

    virtual void observe(const ecs::event::ComponentAdded<ovy::Viewport>& event) override;
    virtual void observe(const ecs::event::ComponentRemoved<ovy::Viewport>& event) override;

    friend class Overlay;
};

} // namespace sys
} // namespace render
} // namespace bl

#endif
