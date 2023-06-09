#ifndef BLIB_RENDER_DRAWABLES_DRAWABLE_HPP
#define BLIB_RENDER_DRAWABLES_DRAWABLE_HPP

#include <BLIB/ECS.hpp>
#include <BLIB/Render/Components/DrawableBase.hpp>
#include <BLIB/Render/Systems/GenericDrawableSystem.hpp>
#include <type_traits>

namespace bl
{
namespace engine
{
class Engine;
}

namespace render
{
/// Collection of SFML-like classes for drawing
namespace draw
{
template<typename TCom, typename TSys = sys::GenericDrawableSystem<TCom>>
class Drawable {
    static_assert(std::is_base_of_v<com::DrawableBase, TCom>, "TCom must derive from DrawableBase");

public:
    Drawable();

    void addToScene(Scene* scene, UpdateSpeed updateFreq);

    void addToSceneWithCustomPipelines(Scene* scene, UpdateSpeed updateFreq,
                                       const scene::StagePipelines& pipelines);

    void addToOverlay(Overlay* overlay, UpdateSpeed descriptorUpdateFreq,
                      ecs::Entity parent = ecs::InvalidEntity);

    void addToOverlayWithCustomPipelines(Overlay* overlay, UpdateSpeed descriptorUpdateFreq,
                                         const scene::StagePipelines& pipelines,
                                         ecs::Entity parent = ecs::InvalidEntity);

    void setHidden(bool hide);

    void removeFromScene();

protected:
    constexpr ecs::Entity entity() const;

    TCom& component();

    const TCom& component() const;

    constexpr engine::Engine& engine();

    template<typename... TArgs>
    void create(engine::Engine& engine, TArgs&&... args);

    void destroy();

private:
    engine::Engine* enginePtr;
    ecs::Entity ecsId;
    ecs::StableHandle<TCom> handle;
};

//////////////////////////// INLINE FUNCTIONS /////////////////////////////////

} // namespace draw
} // namespace render
} // namespace bl

#endif
