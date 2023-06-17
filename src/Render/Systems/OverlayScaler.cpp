#include <BLIB/Render/Systems/OverlayScaler.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>

namespace bl
{
namespace render
{
namespace sys
{
void OverlayScaler::init(engine::Engine& engine) {
    registry = &engine.ecs();
    entities = engine.ecs().getOrCreateView<com::OverlayScaler, t2d::Transform2D>();
    deferredSets.reserve(32);
}

void OverlayScaler::update(std::mutex&, float) {
    entities->forEach([this](ecs::ComponentSet<com::OverlayScaler, t2d::Transform2D>& ent) {
        if (ent.get<com::OverlayScaler>()->dirty) { processEntity(ent); }
    });
    for (SceneSet& set : deferredSets) {
        if (set.sceneRef.object) {
            set.scaler->overlay = dynamic_cast<Overlay*>(set.sceneRef.scene);
            set.scaler->sceneId = set.sceneRef.object->sceneId;
        }
    }
    deferredSets.clear();
}

void OverlayScaler::processOverlaySize(ecs::Entity entity, const glm::u32vec2& targetSize,
                                       const glm::vec2& overlaySize) {
    auto cset = registry->getComponentSet<com::OverlayScaler, t2d::Transform2D>(entity);
    if (cset.isValid()) {
        com::OverlayScaler* scaler = cset.get<com::OverlayScaler>();
        scaler->cachedOverlaySize  = overlaySize;
        scaler->cachedTargetSize =
            glm::vec2(static_cast<float>(targetSize.x), static_cast<float>(targetSize.y));
        processEntity(cset);
    }
}

void OverlayScaler::processEntity(ecs::ComponentSet<com::OverlayScaler, t2d::Transform2D>& ent) {
    com::OverlayScaler& scaler  = *ent.get<com::OverlayScaler>();
    t2d::Transform2D& transform = *ent.get<t2d::Transform2D>();
    scaler.dirty                = false;

    float xScale = 1.f;
    float yScale = 1.f;
    const glm::vec2 targetSize(static_cast<float>(scaler.cachedTargetSize.x),
                               static_cast<float>(scaler.cachedTargetSize.y));

    switch (scaler.scaleType) {
    case com::OverlayScaler::WidthPercent:
        xScale = scaler.cachedOverlaySize.x * scaler.widthPercent / scaler.cachedObjectSize.x;
        yScale =
            targetSize.x * scaler.cachedOverlaySize.y / (targetSize.y * scaler.cachedOverlaySize.x);

        break;

    case com::OverlayScaler::HeightPercent:
        yScale = scaler.cachedOverlaySize.y * scaler.heightPercent / scaler.cachedObjectSize.y;
        xScale = yScale * targetSize.y * scaler.cachedOverlaySize.x /
                 (targetSize.x * scaler.cachedOverlaySize.y);
        break;

    case com::OverlayScaler::SizePercent:
        xScale = scaler.cachedOverlaySize.x * scaler.widthPercent / scaler.cachedObjectSize.x;
        yScale = scaler.cachedOverlaySize.y * scaler.heightPercent / scaler.cachedObjectSize.y;
        break;

    case com::OverlayScaler::PixelRatio:
        xScale = scaler.cachedObjectSize.x * scaler.pixelRatio / targetSize.x *
                 scaler.cachedOverlaySize.x;
        yScale = scaler.cachedObjectSize.y * scaler.pixelRatio / targetSize.y *
                 scaler.cachedOverlaySize.y;
        break;

    case com::OverlayScaler::LineHeight:
        yScale = scaler.overlayRatio;
        xScale = yScale * targetSize.y * scaler.cachedOverlaySize.x /
                 (targetSize.x * scaler.cachedOverlaySize.y);
        break;

    case com::OverlayScaler::None:
    default:
        return;
    }

    transform.setScale({xScale, yScale});
}

void OverlayScaler::queueScalerSceneAdd(draw::base::OverlayScalable* scaler,
                                        const com::SceneObjectRef& sceneRef) {
    // this is a little fragile, ECS may re-alloc pool and invalidate sceneRef before update()
    deferredSets.emplace_back(scaler, sceneRef);
}

} // namespace sys
} // namespace render
} // namespace bl
