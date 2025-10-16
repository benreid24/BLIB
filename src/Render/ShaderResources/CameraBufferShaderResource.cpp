#include <BLIB/Render/ShaderResources/CameraBufferShaderResource.hpp>

#include <BLIB/Engine/Engine.hpp>
#include <BLIB/Render/RenderTarget.hpp>

namespace bl
{
namespace rc
{
namespace sri
{
CameraBufferShaderResource::CameraBufferShaderResource(RenderTarget& owner)
: owner(owner) {}

void CameraBufferShaderResource::init(engine::Engine& engine) {
    BufferShaderResource::init(engine);
    buffer.transferEveryFrame(tfr::Transferable::SyncRequirement::Immediate);
}

void CameraBufferShaderResource::copyFromSource() {
    cam::Camera* camera = owner.getCurrentCamera();
    if (camera) {
        auto& dst          = getBuffer()[0];
        dst.view           = camera->getViewMatrix();
        dst.projection     = camera->getProjectionMatrix(owner.getViewport());
        dst.cameraPosition = camera->getObserverPosition();
    }
}

} // namespace sri
} // namespace rc
} // namespace bl
