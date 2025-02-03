#include <BLIB/Particles/EcsComponent.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace pcl
{
EcsComponent::EcsComponent(engine::Engine& engine, bool transparency) {
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
}

void EcsComponent::makeSprite(const rc::res::TextureRef& texture) {
    vertexBuffer.resize(6, false);

    auto& vertices = vertexBuffer.vertices();

    vertices[0].pos      = {0.f, 0.f, 0.f};
    vertices[0].texCoord = {0.f, 0.f};
    vertices[0].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[1].pos      = {texture->size().x, 0.f, 0.f};
    vertices[1].texCoord = {1.f, 0.f};
    vertices[1].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[2].pos      = {texture->size().x, texture->size().y, 0.f};
    vertices[2].texCoord = {1.f, 1.f};
    vertices[2].color    = {1.f, 1.f, 1.f, 1.f};

    vertices[3] = vertices[0];
    vertices[4] = vertices[1];

    vertices[5].pos      = {0.f, texture->size().y, 0.f};
    vertices[5].texCoord = {0.f, 1.f};
    vertices[5].color    = {1.f, 1.f, 1.f, 1.f};

    for (auto& v : vertices) { v.texCoord = texture->convertCoord(v.texCoord); }

    vertexBuffer.queueTransfer(rc::tfr::Transferable::SyncRequirement::Immediate);
    drawParams.vertexBuffer = vertexBuffer.bufferHandle();
    drawParams.vertexCount  = vertexBuffer.vertices().size();
    syncDrawParamsToScene();
}

} // namespace pcl
} // namespace bl
