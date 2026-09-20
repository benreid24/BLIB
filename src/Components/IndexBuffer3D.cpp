#include <BLIB/Components/IndexBuffer3D.hpp>

#include <BLIB/Engine/Engine.hpp>

namespace bl
{
namespace com
{
void IndexBuffer3D::create(engine::Engine& engine, unsigned int vc, unsigned int ic) {
    buffer.create(engine.renderer(), vc, ic);
    commit();
}

void IndexBuffer3D::resize(unsigned int vc, unsigned int ic) {
    buffer.ensureSize(vc, ic);
    commit();
}

void IndexBuffer3D::commit() {
    drawParams = buffer.getDrawParameters();
    buffer.queueTransfer();
}

} // namespace com
} // namespace bl
