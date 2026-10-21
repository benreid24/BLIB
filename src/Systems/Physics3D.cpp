#include <BLIB/Systems/Physics3D.hpp>

namespace bl
{
namespace sys
{
Physics3D::Physics3D()
: System(engine::FrameStage::Physics, engine::StateMask::Running) {}

void Physics3D::init(engine::Engine& engine) {
    // TODO
}

void Physics3D::update(std::mutex& stageMutex, float dt, float realDt, float residual,
                       float realResidual) {
    // TODO
}

} // namespace sys
} // namespace bl
