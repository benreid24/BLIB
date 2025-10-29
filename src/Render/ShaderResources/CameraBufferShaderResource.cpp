#include <BLIB/Render/ShaderResources/CameraBufferShaderResource.hpp>

#include <BLIB/Cameras/OverlayCamera.hpp>
#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/RenderTarget.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
namespace
{
cam::OverlayCamera overlayCamera;
}

CameraBufferShaderResource::CameraBufferShaderResource(RenderTarget& owner)
: owner(owner)
, isOverlay(false) {}

CameraBufferShaderResource::CameraBufferShaderResource(RenderTarget& owner, Overlay&)
: owner(owner)
, isOverlay(true) {}

void CameraBufferShaderResource::init(engine::Engine& engine) {
    BufferShaderResource::init(engine);
    buffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
}

void CameraBufferShaderResource::copyFromSource() {
    cam::Camera* camera = isOverlay ? &overlayCamera : owner.getCurrentCamera();
    if (camera) {
        auto& dst          = *getBuffer().getWriteAddress();
        dst.view           = camera->getViewMatrix();
        dst.projection     = camera->getProjectionMatrix(owner.getViewport());
        dst.cameraPosition = camera->getObserverPosition();
    }
}

} // namespace sri
} // namespace rc
} // namespace bl
