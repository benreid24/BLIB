#include <BLIB/Components/VertexBuffer3D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace com
{
void VertexBuffer3D::create(engine::Engine& engine, unsigned int vc) {
    buffer.create(engine.renderer(), vc, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    commit();
}

void VertexBuffer3D::resize(unsigned int vc, bool copy) {
    buffer.resize(vc, copy);
    commit();
}

void VertexBuffer3D::commit() {
    drawParams = buffer.getDrawParameters();
    buffer.queueTransfer();
}

} // namespace com
} // namespace bl
