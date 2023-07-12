#include <BLIB/Render/Systems/OverlayScaler.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Render/Drawables/Components/OverlayScalable.hpp>
#include <BLIB/Render/Events/OverlayEntityScaled.hpp>

namespace bl
{
namespace rc
{
namespace sys
{
void OverlayScaler::init(engine::Engine& engine) {
    ignoredEntity = ecs::InvalidEntity;
    registry      = &engine.ecs();
    overlays.reserve(4);
    bl::event::Dispatcher::subscribe(this);
}

void OverlayScaler::update(std::mutex&, float) {
    for (Overlay* o : overlays) { o->refreshScales(); }
}

void OverlayScaler::refreshEntity(ecs::Entity entity, const VkViewport& viewport) {
    auto cset =
        registry->getComponentSet<ecs::Require<com::OverlayScaler, t2d::Transform2D>>(entity);
    if (!cset.isValid()) {
        BL_LOG_ERROR << "Missing components for entity: " << entity;
        return;
    }

    com::OverlayScaler& scaler  = *cset.get<com::OverlayScaler>();
    t2d::Transform2D& transform = *cset.get<t2d::Transform2D>();
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

        registry->emplaceComponent<ovy::Viewport>(
            entity,
            ovy::Viewport::relative({corner.x,
                                     corner.y,
                                     scaler.cachedObjectSize.x * xScale,
                                     scaler.cachedObjectSize.y * yScale}));

        const glm::vec2 newScale{1.f / scaler.cachedObjectSize.x, 1.f / scaler.cachedObjectSize.y};
        transform.setPosition({origin.x * newScale.x, origin.y * newScale.y});
        transform.setScale(newScale);
        ignoredEntity = ecs::InvalidEntity;
    }
    else { transform.setScale({xScale, yScale}); }

    bl::event::Dispatcher::dispatch<event::OverlayEntityScaled>({entity});
}

void OverlayScaler::observe(const ecs::event::ComponentAdded<ovy::Viewport>& event) {
    if (event.entity == ignoredEntity) return;
    com::OverlayScaler* c = registry->getComponent<com::OverlayScaler>(event.entity);
    if (c) { c->dirty = true; }
}

void OverlayScaler::observe(const ecs::event::ComponentRemoved<ovy::Viewport>& event) {
    if (event.entity == ignoredEntity) return;
    com::OverlayScaler* c = registry->getComponent<com::OverlayScaler>(event.entity);
    if (c) { c->dirty = true; }
}

void OverlayScaler::registerOverlay(Overlay* ov) { overlays.emplace_back(ov); }

void OverlayScaler::removeOverlay(Overlay* ov) {
    for (auto it = overlays.begin(); it != overlays.end(); ++it) {
        if (*it == ov) {
            overlays.erase(it);
            return;
        }
    }
}

} // namespace sys
} // namespace rc
} // namespace bl
