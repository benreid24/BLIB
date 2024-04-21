#include <BLIB/Particles/EcsComponent.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace pcl
{
EcsComponent::EcsComponent(engine::Engine& engine, bool transparency, std::uint32_t pipeline) {
    vertexBuffer.create(engine.renderer().vulkanState(), 1);

    auto& vertices       = vertexBuffer.vertices();
    vertices[0].pos      = {0.f, 0.f, 0.f};
    vertices[0].texCoord = {0.f, 0.f};
    vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

    vertexBuffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);

    drawParams               = vertexBuffer.getDrawParameters();
    drawParams.instanceCount = 0;
    drawParams.firstInstance = 0;
    containsTransparency     = transparency;
    this->pipeline           = pipeline;
}

} // namespace pcl
} // namespace bl
