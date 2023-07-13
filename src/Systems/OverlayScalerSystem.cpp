#include <BLIB/Systems/OverlayScalerSystem.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
#include <BLIB/Render/Events/OverlayEntityScaled.hpp>
#include <BLIB/Render/Overlays/Overlay.hpp>

namespace bl
{
namespace sys
{
void OverlayScalerSystem::init(engine::Engine& engine) {
    ignoredEntity = ecs::InvalidEntity;
    registry      = &engine.ecs();
    overlays.reserve(4);
    bl::event::Dispatcher::subscribe(this);
}

void OverlayScalerSystem::update(std::mutex&, float) {
    for (rc::Overlay* o : overlays) { o->refreshScales(); }
}

void OverlayScalerSystem::refreshEntity(ecs::Entity entity, const VkViewport& viewport) {
    auto cset =
        registry->getComponentSet<ecs::Require<com::OverlayScaler, com::Transform2D>>(entity);
    if (!cset.isValid()) {
        BL_LOG_ERROR << "Missing components for entity: " << entity;
        return;
    }

    com::OverlayScaler& scaler  = *cset.get<com::OverlayScaler>();
    com::Transform2D& transform = *cset.get<com::Transform2D>();
    scaler.dirty                = false;
    scaler.cachedTargetRegion =
        sf::FloatRect{viewport.x, viewport.y, viewport.width, viewport.height};

    float xScale = 1.f;
    float yScale = 1.f;

    switch (scaler.scaleType) {
    case com::OverlayScaler::WidthPercent:
        xScale = scaler.widthPercent / scaler.cachedObjectSize.x;
        yScale = xScale * viewport.width / viewport.height;
        break;

    case com::OverlayScaler::HeightPercent:
        yScale = scaler.heightPercent / scaler.cachedObjectSize.y;
        xScale = yScale * viewport.height / viewport.width;
        break;

    case com::OverlayScaler::SizePercent:
        xScale = scaler.widthPercent / scaler.cachedObjectSize.x;
        yScale = scaler.heightPercent / scaler.cachedObjectSize.y;
        break;

    case com::OverlayScaler::PixelRatio:
        xScale = scaler.cachedObjectSize.x * scaler.pixelRatio / viewport.width;
        yScale = scaler.cachedObjectSize.y * scaler.pixelRatio / viewport.height;
        break;

    case com::OverlayScaler::LineHeight:
        yScale = scaler.overlayRatio;
        xScale = yScale * viewport.height / viewport.width;
        break;

    case com::OverlayScaler::None:
    default:
        return;
    }

    if (scaler.useViewport) {
        ignoredEntity           = entity;
        const glm::vec2 pos     = scaler.ogPos.value_or(transform.getPosition());
        const glm::vec2& origin = transform.getOrigin();
        const glm::vec2 offset(origin.x * xScale, origin.y * yScale);
        const glm::vec2 corner = pos - offset;

        if (!scaler.ogPos.has_value()) {
            // TODO - how to handle setPosition now?
            // possible: use existing viewport to map back into parent space
            scaler.ogPos = transform.getPosition();
        }

        registry->emplaceComponent<rc::ovy::Viewport>(
            entity,
            rc::ovy::Viewport::relative({corner.x,
                                         corner.y,
                                         scaler.cachedObjectSize.x * xScale,
                                         scaler.cachedObjectSize.y * yScale}));

        const glm::vec2 newScale{1.f / scaler.cachedObjectSize.x, 1.f / scaler.cachedObjectSize.y};
        transform.setPosition({origin.x * newScale.x, origin.y * newScale.y});
        transform.setScale(newScale);
        ignoredEntity = ecs::InvalidEntity;
    }
    else { transform.setScale({xScale, yScale}); }

    bl::event::Dispatcher::dispatch<rc::event::OverlayEntityScaled>({entity});
}

void OverlayScalerSystem::observe(const ecs::event::ComponentAdded<rc::ovy::Viewport>& event) {
    if (event.entity == ignoredEntity) return;
    com::OverlayScaler* c = registry->getComponent<com::OverlayScaler>(event.entity);
    if (c) { c->dirty = true; }
}

void OverlayScalerSystem::observe(const ecs::event::ComponentRemoved<rc::ovy::Viewport>& event) {
    if (event.entity == ignoredEntity) return;
    com::OverlayScaler* c = registry->getComponent<com::OverlayScaler>(event.entity);
    if (c) { c->dirty = true; }
}

void OverlayScalerSystem::registerOverlay(rc::Overlay* ov) { overlays.emplace_back(ov); }

void OverlayScalerSystem::removeOverlay(rc::Overlay* ov) {
    for (auto it = overlays.begin(); it != overlays.end(); ++it) {
        if (*it == ov) {
            overlays.erase(it);
            return;
        }
    }
}

} // namespace sys
} // namespace bl
