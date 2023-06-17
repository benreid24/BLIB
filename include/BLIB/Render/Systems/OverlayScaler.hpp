#ifndef BLIB_RENDER_SYSTEMS_OVERLAYSCALER_HPP
#define BLIB_RENDER_SYSTEMS_OVERLAYSCALER_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Engine/System.hpp>
#include <BLIB/Render/Components/OverlayScaler.hpp>
#include <BLIB/Render/Components/SceneObjectRef.hpp>
#include <BLIB/Transforms/2D/Transform2D.hpp>
#include <vector>

namespace bl
{
namespace render
{
namespace draw
{
namespace base
{
class OverlayScalable;
}
} // namespace draw

namespace sys
{
/**
 * @brief System that processes entities that are scaled for overlays. Keeps scales up-to-date as
 *        overlay size changes
 *
 * @ingroup Renderer
 */
class OverlayScaler : public engine::System {
public:
    /**
     * @brief Creates the system
     */
    OverlayScaler() = default;

    /**
     * @brief Destroys the system
     */
    virtual ~OverlayScaler() = default;

    /**
     * @brief Handles a resize of an overlay. Intended to be called by Overlay
     *
     * @param overlay The entity that is affected
     * @param targetSize The size of target area being rendered to
     * @param overlaySize The virtual coordinate size of the overlay camera
     */
    void processOverlaySize(ecs::Entity entity, const glm::u32vec2& targetSize,
                            const glm::vec2& overlaySize);

private:
    struct SceneSet {
        SceneSet(draw::base::OverlayScalable* scaler, const com::SceneObjectRef& sceneRef)
        : scaler(scaler)
        , sceneRef(sceneRef) {}

        draw::base::OverlayScalable* scaler;
        const com::SceneObjectRef& sceneRef;
    };

    ecs::Registry* registry;
    ecs::View<com::OverlayScaler, t2d::Transform2D>* entities;
    std::vector<SceneSet> deferredSets;

    virtual void init(engine::Engine& engine) override;
    virtual void update(std::mutex& stageMutex, float dt) override;

    void processEntity(ecs::ComponentSet<com::OverlayScaler, t2d::Transform2D>& entity);
    void queueScalerSceneAdd(draw::base::OverlayScalable* scaler,
                             const com::SceneObjectRef& sceneRef);

    friend class draw::base::OverlayScalable;
};

} // namespace sys
} // namespace render
} // namespace bl

#endif
