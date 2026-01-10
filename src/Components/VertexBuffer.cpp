#include <BLIB/Components/VertexBuffer.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace com
{
void VertexBuffer::create(engine::Engine& engine, unsigned int vc) {
    buffer.create(engine.renderer(), vc, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    commit();
}

void VertexBuffer::resize(unsigned int vc, bool copy) {
    buffer.resize(vc, copy);
    commit();
}

void VertexBuffer::commit() {
    drawParams = buffer.getDrawParameters();
    buffer.queueTransfer();
}

} // namespace com
} // namespace bl
