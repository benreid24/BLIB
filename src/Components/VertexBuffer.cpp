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

void VertexBuffer::resize(unsigned int vc, bool copy) {
    buffer.resize(vc, copy);
    commit();
}

void VertexBuffer::commit() {
    drawParams = buffer.getDrawParameters();
    buffer.queueTransfer();
    syncDrawParamsToScene();
}

} // namespace com
} // namespace bl
