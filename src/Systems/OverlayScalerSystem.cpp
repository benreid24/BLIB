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
bool parentsAllClean(com::OverlayScaler* scaler, com::Transform2D* transform) {
    while (scaler->hasParent()) {
        scaler    = &scaler->getParent();
        transform = (transform && transform->hasParent()) ? &transform->getParent() : nullptr;
        if (scaler->isDirty(transform)) { return false; }
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

    const unsigned int limitRight = limits.offset.x + limits.extent.width;
    if (scissor.offset.x + scissor.extent.width > limitRight) {
        if (limitRight >= scissor.offset.x) {
            scissor.extent.width = limitRight - scissor.offset.x;
        }
        else { scissor.extent.width = 0; }
    }

    const unsigned int limitBottom = limits.offset.y + limits.extent.height;
    if (scissor.offset.y + scissor.extent.height > limitBottom) {
        if (limitBottom >= scissor.offset.y) {
            scissor.extent.height = limitBottom - scissor.offset.y;
        }
        else { scissor.extent.height = 0; }
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
        com::OverlayScaler* scaler  = row.get<com::OverlayScaler>();
        com::Transform2D* transform = row.get<com::Transform2D>();
        if (scaler && scaler->isDirty(transform)) {
            // skip if parent dirty, will be refreshed when parent is
            if (!parentsAllClean(scaler, transform)) { return; }
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
    else if (childRow.get<rc::ovy::OverlayObject>()) {
        rc::ovy::OverlayObject& obj = *childRow.get<rc::ovy::OverlayObject>();
        if (obj.overlay && obj.overlayViewport) {
            obj.cachedScissor = makeScissor(*obj.overlayViewport);
        }
    }
    else { BL_LOG_ERROR << "Invalid child entity: " << obj.entity; }
}

void OverlayScalerSystem::refreshEntity(Result& cset) {
    com::OverlayScaler& scaler  = *cset.get<com::OverlayScaler>();
    com::Transform2D& transform = *cset.get<com::Transform2D>();
    rc::ovy::OverlayObject& obj = *cset.get<rc::ovy::OverlayObject>();

    scaler.dirty            = false;
    scaler.transformVersion = transform.getVersion();

    if (!obj.overlayViewport) { return; }

    // dummy entities only get scissor, all else is skipped
    if (cset.entity().flagSet(ecs::Flags::Dummy)) {
        updateScissor(cset, 1.f, 1.f);
        return;
    }

    const VkViewport& viewport = *obj.overlayViewport;
    glm::vec2 parentSize       = cam::OverlayCamera::getOverlayCoordinateSpace();
    if (scaler.hasParent()) {
        parentSize.x =
            scaler.getParent().cachedObjectBounds.width * transform.getParent().getScale().x;
        parentSize.y =
            scaler.getParent().cachedObjectBounds.height * transform.getParent().getScale().y;
    }

    float xScale = 1.f;
    float yScale = 1.f;

    // scale
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

    if (scaler.scaleType != com::OverlayScaler::None) {
        if (transform.getScale().x != xScale || transform.getScale().y != yScale) {
            transform.setScale({xScale, yScale});
            if (scaler.onScale) { scaler.onScale(); }
        }
    }
    else {
        xScale = transform.getScale().x;
        yScale = transform.getScale().y;
    }

    // position
    switch (scaler.posType) {
    case com::OverlayScaler::ParentSpace:
        transform.setPosition(scaler.parentPosition * parentSize);
        break;

    case com::OverlayScaler::NoPosition:
    default:
        break;
    }

    // scissor
    updateScissor(cset, xScale, yScale);

    // update global size
    const glm::vec2 pos          = transform.getGlobalPosition();
    const glm::vec2& overlaySize = cam::OverlayCamera::getOverlayCoordinateSpace();
    scaler.cachedTargetRegion    = {
        viewport.x + viewport.width * pos.x / overlaySize.x,
        viewport.y + viewport.height * pos.y / overlaySize.y,
        viewport.width * (scaler.cachedObjectBounds.width * xScale) / overlaySize.x,
        viewport.height * (scaler.cachedObjectBounds.height * yScale) / overlaySize.y};
}

void OverlayScalerSystem::updateScissor(Result& cset, float xScale, float yScale) {
    rc::ovy::OverlayObject& obj = *cset.get<rc::ovy::OverlayObject>();
    VkRect2D& scissor           = obj.cachedScissor;
    com::Transform2D& transform = *cset.get<com::Transform2D>();
    com::OverlayScaler& scaler  = *cset.get<com::OverlayScaler>();
    const glm::vec2 pos         = transform.getGlobalPosition();

    switch (scaler.scissorMode) {
    case com::OverlayScaler::ScissorSelf:
    case com::OverlayScaler::ScissorSelfConstrained:
        if (obj.overlayViewport) {
            const VkViewport& viewport = *obj.overlayViewport;
            const glm::vec2& origin    = transform.getOrigin();
            const glm::vec2 offset((origin.x - scaler.cachedObjectBounds.left) * xScale,
                                   (origin.y - scaler.cachedObjectBounds.top) * yScale);
            const glm::vec2 corner =
                (pos - offset) / cam::OverlayCamera::getOverlayCoordinateSpace();

            scissor.offset.x     = viewport.x + viewport.width * corner.x;
            scissor.offset.y     = viewport.y + viewport.height * corner.y;
            scissor.extent.width = viewport.width * (scaler.cachedObjectBounds.width * xScale) /
                                   cam::OverlayCamera::getOverlayCoordinateSpace().x;
            scissor.extent.height = viewport.height * (scaler.cachedObjectBounds.height * yScale) /
                                    cam::OverlayCamera::getOverlayCoordinateSpace().y;

            VkRect2D limits = makeScissor(viewport);
            if (scaler.scissorMode == com::OverlayScaler::ScissorSelfConstrained &&
                obj.hasParent()) {
                limits = obj.getParent().cachedScissor;
            }
            constrainScissor(scissor, limits);
        }
        else { BL_LOG_ERROR << "Dummy entities do not support ScissorSelf modes"; }
        break;

    case com::OverlayScaler::ScissorObserver:
        if (obj.overlayViewport) { scissor = makeScissor(*obj.overlayViewport); }
        else { BL_LOG_ERROR << "Dummy entities do not support ScissorObserver mode"; }
        break;

    case com::OverlayScaler::ScissorInherit:
        if (!obj.hasParent()) {
            if (obj.overlayViewport) { obj.cachedScissor = makeScissor(*obj.overlayViewport); }
        }
        else { obj.cachedScissor = obj.getParent().cachedScissor; }
        break;

    case com::OverlayScaler::ScissorFixed:
        scissor.offset.x      = scaler.fixedScissor.left;
        scissor.offset.y      = scaler.fixedScissor.top;
        scissor.extent.width  = scaler.fixedScissor.width;
        scissor.extent.height = scaler.fixedScissor.height;
        break;
    }
}

} // namespace sys
} // namespace bl
