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
    registry = &engine.ecs();
    overlays.reserve(4);
    view       = engine.ecs().getOrCreateView<Required, Optional>();
    scalerPool = &engine.ecs().getAllComponents<com::OverlayScaler>();
}

void OverlayScalerSystem::update(std::mutex&, float, float, float, float) {
    view->forEach([this](Result& row) {
        com::OverlayScaler* scaler = row.get<com::OverlayScaler>();
        if (scaler && scaler->isDirty()) {
            // skip if parent dirty, will be refreshed when parent is
            if (!parentsAllClean(scaler)) { return; }

            const rc::ovy::OverlayObject& obj = *row.get<rc::ovy::OverlayObject>();
            refreshObjectAndChildren(row);
        }
    });
}

void OverlayScalerSystem::refreshObjectAndChildren(Result& row) {
    refreshEntity(row);
    for (rc::ovy::OverlayObject* child : row.get<rc::ovy::OverlayObject>()->getChildren()) {
        refreshObjectAndChildren(*child);
    }
}

void OverlayScalerSystem::refreshObjectAndChildren(rc::ovy::OverlayObject& obj) {
    Result childRow = registry->getComponentSet<Required, Optional>(obj.entity);
    if (childRow.isValid()) { refreshObjectAndChildren(childRow); }
    else { BL_LOG_ERROR << "Invalid child entity: " << obj.entity; }
}

void OverlayScalerSystem::refreshEntity(Result& cset) {
    com::OverlayScaler& scaler  = *cset.get<com::OverlayScaler>();
    com::Transform2D& transform = *cset.get<com::Transform2D>();
    const VkViewport& viewport  = *cset.get<rc::ovy::OverlayObject>()->overlayViewport;

    scaler.dirty              = false;
    scaler.cachedTargetRegion = {viewport.x, viewport.y, viewport.width, viewport.height};

    float xScale = 1.f;
    float yScale = 1.f;

    // TODO - update scale calculations to parent size
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
    if (scaler.useScissor) {
        const glm::vec2 pos     = transform.getGlobalPosition();
        const glm::vec2& origin = transform.getOrigin();
        const glm::vec2 offset(origin.x * xScale, origin.y * yScale);
        const glm::vec2 corner = pos - offset;

        VkRect2D& scissor     = cset.get<rc::ovy::OverlayObject>()->cachedScissor;
        scissor.offset.x      = viewport.x + viewport.width * corner.x;
        scissor.offset.y      = viewport.y + viewport.height * corner.y;
        scissor.extent.width  = viewport.width * scaler.cachedObjectSize.x * xScale;
        scissor.extent.height = viewport.height * scaler.cachedObjectSize.y * yScale;
        // TODO - constrain child scissors to parent? make option? (dropdown extend past window)
    }
    else {
        scaleChanged = transform.getScale().x != xScale || transform.getScale().y != yScale;
        transform.setScale({xScale, yScale});

        // ensure scissor is updated
        VkRect2D& scissor     = cset.get<rc::ovy::OverlayObject>()->cachedScissor;
        scissor.offset.x      = viewport.x;
        scissor.offset.y      = viewport.y;
        scissor.extent.width  = viewport.width;
        scissor.extent.height = viewport.height;
    }

    if (scaleChanged) {
        bl::event::Dispatcher::dispatch<rc::event::OverlayEntityScaled>({cset.entity()});
    }
}

} // namespace sys
} // namespace bl
