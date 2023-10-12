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
    glm::vec2 parentSize        = glm::vec2{1.f, 1.f};
    if (scaler.hasParent()) {
        parentSize.x = scaler.getParent().cachedObjectSize.x * transform.getParent().getScale().x;
        parentSize.y = scaler.getParent().cachedObjectSize.y * transform.getParent().getScale().y;
    }

    scaler.dirty              = false;
    scaler.cachedTargetRegion = {viewport.x, viewport.y, viewport.width, viewport.height};

    float xScale = 1.f;
    float yScale = 1.f;

    switch (scaler.scaleType) {
    case com::OverlayScaler::WidthPercent:
        xScale = scaler.widthPercent * parentSize.x / scaler.cachedObjectSize.x;
        yScale = xScale * viewport.width / viewport.height;
        break;

    case com::OverlayScaler::HeightPercent:
        yScale = scaler.heightPercent * parentSize.y / scaler.cachedObjectSize.y;
        xScale = yScale * viewport.height / viewport.width;
        break;

    case com::OverlayScaler::SizePercent:
        xScale = scaler.widthPercent * parentSize.x / scaler.cachedObjectSize.x;
        yScale =
            scaler.heightPercent * (parentSize.y / viewport.height) / scaler.cachedObjectSize.y;
        break;

    case com::OverlayScaler::PixelRatio:
        xScale = scaler.cachedObjectSize.x * scaler.pixelRatio / viewport.width;
        yScale = scaler.cachedObjectSize.y * scaler.pixelRatio / viewport.height;
        break;

    case com::OverlayScaler::LineHeight:
        yScale = scaler.overlayRatio * parentSize.y;
        xScale = yScale * viewport.height / viewport.width;
        break;

    case com::OverlayScaler::None:
    default:
        return;
    }

    if (transform.getScale().x != xScale || transform.getScale().y != yScale) {
        transform.setScale({xScale, yScale});
        bl::event::Dispatcher::dispatch<rc::event::OverlayEntityScaled>({cset.entity()});
    }

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
        // TODO - constrain child scissors to parent? make option? (selection extend past window)
    }
    else {
        // ensure scissor is updated
        auto& obj = *cset.get<rc::ovy::OverlayObject>();
        if (!obj.hasParent()) {
            VkRect2D& scissor     = obj.cachedScissor;
            scissor.offset.x      = viewport.x;
            scissor.offset.y      = viewport.y;
            scissor.extent.width  = viewport.width;
            scissor.extent.height = viewport.height;
        }
        else {
            // TODO - make this optional?
            obj.cachedScissor = obj.getParent().cachedScissor;
        }
    }

    switch (scaler.posType) {
    case com::OverlayScaler::ParentSpace:
        transform.setPosition(scaler.parentPosition * parentSize);
        break;

    case com::OverlayScaler::NoPosition:
    default:
        break;
    }
}

} // namespace sys
} // namespace bl
