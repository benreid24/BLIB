#include <BLIB/Components/VertexBuffer.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace com
{
void VertexBuffer::create(engine::Engine& engine, unsigned int vc) {
    buffer.create(engine.renderer().vulkanState(), vc);
    commit();
}

void VertexBuffer::commit() {
    drawParams = buffer.getDrawParameters();
    buffer.queueTransfer();
    if (sceneRef.object) { syncDrawParamsToScene(); }
}

} // namespace com
} // namespace bl
