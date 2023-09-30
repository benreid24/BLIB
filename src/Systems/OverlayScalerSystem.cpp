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
namespace
{
bool parentsAllClean(com::OverlayScaler* scaler) {
    while (scaler->hasParent()) {
        scaler = &scaler->getParent();
        if (scaler->isDirty()) { return false; }
    }
    return true;
}

} // namespace

void OverlayScalerSystem::init(engine::Engine& engine) {
    ignoredEntity = ecs::InvalidEntity;
    registry      = &engine.ecs();
    overlays.reserve(4);
    view       = engine.ecs().getOrCreateView<Required, Optional>();
    scalerPool = &engine.ecs().getAllComponents<com::OverlayScaler>();
    bl::event::Dispatcher::subscribe(this);
}

void OverlayScalerSystem::update(std::mutex&, float) {
    view->forEach([this](Result& row) {
        com::OverlayScaler* scaler = row.get<com::OverlayScaler>();
        if (scaler && scaler->isDirty()) {
            // skip if parent dirty, will be refreshed when parent is
            if (!parentsAllClean(scaler)) { return; }

            const rc::ovy::OverlayObject& obj = *row.get<rc::ovy::OverlayObject>();
            const VkViewport& pvp =
                obj.hasParent() ? obj.getParent().cachedViewport : *obj.overlayViewport;
            refreshObjectAndChildren(row, pvp);
        }
    });
}

void OverlayScalerSystem::refreshObjectAndChildren(Result& row, const VkViewport& viewport) {
    refreshEntity(row, viewport);
    row.get<rc::ovy::OverlayObject>()->refreshViewport(
        registry->getComponent<rc::ovy::Viewport>(row.entity()), viewport);
    const auto& nvp = row.get<rc::ovy::OverlayObject>()->cachedViewport;
    for (rc::ovy::OverlayObject* child : row.get<rc::ovy::OverlayObject>()->getChildren()) {
        refreshObjectAndChildren(*child, nvp);
    }
}

void OverlayScalerSystem::refreshObjectAndChildren(rc::ovy::OverlayObject& obj,
                                                   const VkViewport& viewport) {
    Result childRow = registry->getComponentSet<Required, Optional>(obj.entity);
    if (childRow.isValid()) { refreshObjectAndChildren(childRow, viewport); }
    else { BL_LOG_ERROR << "Invalid child entity: " << obj.entity; }
}

void OverlayScalerSystem::refreshEntity(Result& cset, const VkViewport& viewport) {
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

    bool scaleChanged = false;
    if (scaler.useViewport) {
        ignoredEntity           = cset.entity();
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
            cset.entity(),
            rc::ovy::Viewport::relative({corner.x,
                                         corner.y,
                                         scaler.cachedObjectSize.x * xScale,
                                         scaler.cachedObjectSize.y * yScale}));

        const glm::vec2 newScale{1.f / scaler.cachedObjectSize.x, 1.f / scaler.cachedObjectSize.y};
        scaleChanged = transform.getScale().x != xScale || transform.getScale().y != yScale;
        transform.setPosition({origin.x * newScale.x, origin.y * newScale.y});
        transform.setScale(newScale);
        ignoredEntity = ecs::InvalidEntity;
    }
    else {
        scaleChanged = transform.getScale().x != xScale || transform.getScale().y != yScale;
        transform.setScale({xScale, yScale});
    }

    if (scaleChanged) {
        bl::event::Dispatcher::dispatch<rc::event::OverlayEntityScaled>({cset.entity()});
    }
}

void OverlayScalerSystem::observe(const ecs::event::ComponentAdded<rc::ovy::Viewport>& event) {
    if (event.entity == ignoredEntity) return;
    com::OverlayScaler* c = scalerPool->get(event.entity);
    if (c) { c->dirty = true; }
}

void OverlayScalerSystem::observe(const ecs::event::ComponentRemoved<rc::ovy::Viewport>& event) {
    if (event.entity == ignoredEntity) return;
    com::OverlayScaler* c = scalerPool->get(event.entity);
    if (c) { c->dirty = true; }
}

} // namespace sys
} // namespace bl
