#include <BLIB/Systems/OverlayScalerSystem.hpp>

#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Events.hpp>
#include <BLIB/Graphics/Components/OverlayScalable.hpp>
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

VkRect2D makeScissor(const VkViewport& vp) {
    VkRect2D scissor{};
    scissor.offset.x      = vp.x;
    scissor.offset.y      = vp.y;
    scissor.extent.width  = vp.width;
    scissor.extent.height = vp.height;
    return scissor;
}

void constrainScissor(VkRect2D& scissor, const VkRect2D& limits) {
    scissor.offset.x = std::max(scissor.offset.x, limits.offset.x);
    scissor.offset.y = std::max(scissor.offset.y, limits.offset.y);

    const auto limitRight = limits.offset.x + limits.extent.width;
    if (scissor.offset.x + scissor.extent.width > limitRight) {
        scissor.extent.width = limitRight - scissor.offset.x;
    }

    const auto limitBottom = limits.offset.y + limits.extent.height;
    if (scissor.offset.y + scissor.extent.height > limitBottom) {
        scissor.extent.height = limitBottom - scissor.offset.y;
    }
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
    glm::vec2 parentSize        = cam::OverlayCamera::getOverlayCoordinateSpace();
    if (scaler.hasParent()) {
        parentSize.x =
            scaler.getParent().cachedObjectBounds.width * transform.getParent().getScale().x;
        parentSize.y =
            scaler.getParent().cachedObjectBounds.height * transform.getParent().getScale().y;
    }

    scaler.dirty = false;

    float xScale = 1.f;
    float yScale = 1.f;

    switch (scaler.scaleType) {
    case com::OverlayScaler::WidthPercent:
        xScale = scaler.widthPercent * parentSize.x / scaler.cachedObjectBounds.width;
        yScale = xScale;
        break;

    case com::OverlayScaler::HeightPercent:
        yScale = scaler.heightPercent * parentSize.y / scaler.cachedObjectBounds.height;
        xScale = yScale;
        break;

    case com::OverlayScaler::SizePercent:
        xScale = scaler.widthPercent * parentSize.x / scaler.cachedObjectBounds.width;
        yScale = scaler.heightPercent * parentSize.y / scaler.cachedObjectBounds.height;
        break;

    case com::OverlayScaler::PixelRatio:
        xScale = scaler.cachedObjectBounds.width * scaler.pixelRatio / viewport.width;
        yScale = scaler.cachedObjectBounds.height * scaler.pixelRatio / viewport.height;
        break;

    case com::OverlayScaler::LineHeight:
        yScale = scaler.overlayRatio * parentSize.y;
        xScale = yScale;
        break;

    case com::OverlayScaler::None:
    default:
        break;
    }

    if (transform.getScale().x != xScale || transform.getScale().y != yScale) {
        transform.setScale({xScale, yScale});
        if (scaler.onScale) { scaler.onScale(); }
    }

    switch (scaler.posType) {
    case com::OverlayScaler::ParentSpace:
        transform.setPosition(scaler.parentPosition * parentSize);
        break;

    case com::OverlayScaler::NoPosition:
    default:
        break;
    }

    const glm::vec2 pos = transform.getGlobalPosition();
    if (scaler.useScissor) {
        const glm::vec2& origin = transform.getOrigin();
        const glm::vec2 offset((origin.x - scaler.cachedObjectBounds.left) * xScale,
                               (origin.y - scaler.cachedObjectBounds.top) * yScale);
        const glm::vec2 corner = (pos - offset) / parentSize;

        VkRect2D& scissor = cset.get<rc::ovy::OverlayObject>()->cachedScissor;
        scissor.offset.x  = viewport.x + viewport.width * corner.x;
        scissor.offset.y  = viewport.y + viewport.height * corner.y;
        scissor.extent.width =
            viewport.width * (scaler.cachedObjectBounds.width * xScale) / parentSize.x;
        scissor.extent.height =
            viewport.height * (scaler.cachedObjectBounds.height * yScale) / parentSize.y;
        constrainScissor(scissor, makeScissor(viewport));
        // TODO - constrain child scissors to parent? make option? (selection extend past window)
    }
    else {
        // ensure scissor is updated
        auto& obj = *cset.get<rc::ovy::OverlayObject>();
        if (!obj.hasParent()) { obj.cachedScissor = makeScissor(viewport); }
        else {
            // TODO - make this optional?
            obj.cachedScissor = obj.getParent().cachedScissor;
        }
    }

    const glm::vec2& overlaySize = cam::OverlayCamera::getOverlayCoordinateSpace();
    scaler.cachedTargetRegion    = {
        viewport.x + viewport.width * pos.x / overlaySize.x,
        viewport.y + viewport.height * pos.y / overlaySize.y,
        viewport.width * (scaler.cachedObjectBounds.width * xScale) / overlaySize.x,
        viewport.height * (scaler.cachedObjectBounds.height * yScale) / overlaySize.y};
}

} // namespace sys
} // namespace bl
